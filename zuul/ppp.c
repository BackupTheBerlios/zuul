/* ppp.c
 * 
 * Copyright (c) 2004 Peter Gerbrandt
 *
 * based on ppp.c from the diald project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include <sys/wait.h>

#if 0
#ifdef PPP_VERSION_2_2_0
#include <linux/ppp_defs.h>
#include <linux/if_ppp.h>
#else
#include <linux/ppp.h>
#endif
#else
#define PPPIOCGUNIT_2_1_2 0x5494
#define PPPIOCGUNIT_2_2_0 _IOR('t', 86, int)
#endif

#include "zuul.h"
#include "ppp.h"
#include "list.h"
#include "options.h"

static char *RCSID="$Id: ppp.c,v 1.1 2004/03/05 00:10:49 systor Exp $";

void ppp_start(struct device_list *entry)
{
	int pgrpid;
	
	/* Run pppd directly here and set up to wait for the iface */
	entry->status = 1;
	entry->link_pid = fork();


	if (entry->link_pid < 0) {
		syslog(LOG_ERR, "failed to fork pppd: %m");
		exit(EXIT_FAILURE);
	}

#define ADD_ARG(arg) { argv[i] = arg; argv_len += strlen(argv[i++]) + 1; }
    
	if (entry->link_pid == 0) {
		char **argv = (char **)malloc(sizeof(char *)*(pppd_argc+12));
		int argv_len = 0;
		char buf[24], tmpbuf[50], *argv_buf, connect[70];
		int i = 0, j;

		ADD_ARG(path_pppd);
		ADD_ARG(entry->device);
		ADD_ARG("-defaultroute");
		ADD_ARG("-detach");
		if (modem){
			ADD_ARG("modem");
			ADD_ARG("connect");
			sprintf(connect, 
// should add ABORT 'BLACKLISTED' here
// pppd dies when it is set
				"/usr/sbin/chat -v ABORT 'BUSY' ABORT "
				"'NO CARRIER' '' ATZ OK  ATDT%s CONNECT",
				iptotel((char *)inet_ntoa(*entry->raddr)));
			ADD_ARG(connect);
		}
		else{
			ADD_ARG("local");
		}
		ADD_ARG("115200");
		sprintf(buf, "%s:", inet_ntoa(*entry->laddr));
		sprintf(tmpbuf, "%s%s", buf, inet_ntoa(*entry->raddr));
		ADD_ARG(tmpbuf);
//		if (crtscts) ADD_ARG("crtscts");
//		ADD_ARG("mtu");
//		sprintf(buf,"%d",mtu);
//		ADD_ARG(strdup(buf));
//		ADD_ARG("mru");
//		sprintf(buf,"%d",mru);
//		ADD_ARG(strdup(buf));
//		if (netmask) {
//			ADD_ARG("netmask");
//			ADD_ARG(netmask);
//		}
		for (j = 0; j < pppd_argc; j++) {
			ADD_ARG(pppd_argv[j]);
		}
		argv[i++] = 0;

		if ((argv_buf = (char *)malloc(argv_len + 1))) {
			argv_len = i - 1;
			*argv_buf = '\0';
			for (i = 0; i < argv_len; i++) {
				strcat(argv_buf, argv[i]);
				strcat(argv_buf, " ");
			}
			syslog(LOG_DEBUG, "Running pppd: %s", argv_buf);
		}

		/* make sure pppd is the session leader and has the controlling
		 * terminal so it gets the SIGHUP's
		 */
		pgrpid = setsid();
		ioctl(modem_fd, TIOCSCTTY, 1);
		tcsetpgrp(modem_fd, pgrpid);

		setreuid(getuid(), getuid());
		setregid(getgid(), getgid());

		if (modem_fd != 0)
			dup2(modem_fd, 0);
		else
			fcntl(modem_fd, F_SETFD, 0);
		dup2(0, 1);

		execv(path_pppd,argv);

		syslog(LOG_ERR, "could not exec %s: %m",path_pppd);
		_exit(99);
		/* NOTREACHED */
	}
	syslog(LOG_INFO,"Running pppd (pid = %d).",entry->link_pid);
}

void ppp_return_code_resolve(int status, pid_t pid)
{
	switch (WEXITSTATUS(status)) {
	case 0:
		syslog(LOG_ERR, "PPPD[%d] exited normally.", pid);
		break;
	case 1:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "An immediately fatal error of some kind occurred, "
		       "such as an essential system call failing, "
		       "or running out of virtual memory.",
		       pid, WEXITSTATUS(status));
		break;
	case 2:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "An error was detected in processing the options "
		       "given, such as two mutually exclusive options "
		       "being used.",
		       pid, WEXITSTATUS(status));
		break;
	case 3:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "Pppd is not setuid-root and the invoking user "
		       "is not root.",
		       pid, WEXITSTATUS(status));
		break;
	case 4:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "The kernel does not support PPP, for example, "
		       "the PPP kernel driver is not included or cannot "
		       "be loaded.",
		       pid, WEXITSTATUS(status));
		break;
	case 5:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "Pppd terminated because it was sent a SIGINT, "
		       "SIGTERM or SIGHUP signal.",
		       pid, WEXITSTATUS(status));
		break;
	case 6:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "The serial port could not be locked.",
		       pid, WEXITSTATUS(status));
		break;
	case 7:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "The serial port could not be opened.",
		       pid, WEXITSTATUS(status));
		break;
	case 8:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "The connect script failed (returned a non-zero "
		       "exit status).",
		       pid, WEXITSTATUS(status));
		break;
	case 9:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "The command specified as the argument to the pty "
		       "option could not be run.",
		       pid, WEXITSTATUS(status));
		break;
	case 10:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "The PPP negotiation failed, that is, "
		       "it didn't reach the point where at least "
		       "one network protocol (e.g. IP) was running.",
		       pid, WEXITSTATUS(status));
		break;
	case 11:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "The peer system failed (or refused) to "
		       "authenticate itself.",
		       pid, WEXITSTATUS(status));
		break;
	case 12:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "idle timeout",
		       pid, WEXITSTATUS(status));
		break;
	case 13:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "connect time limit reached",
		       pid, WEXITSTATUS(status));
		break;
	case 14:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "callback requested",
		       pid, WEXITSTATUS(status));
		break;
	case 15:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "peer doesn't respond to echo requests",
		       pid, WEXITSTATUS(status));
		break;
	case 16:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "modem hangup",
		       pid, WEXITSTATUS(status));
		break;
	case 17:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "The PPP negotiation failed because serial "
		       "loopback was detected.",
		       pid, WEXITSTATUS(status));
		break;
	case 18:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "The init script failed (returned a non-zero "
		       "exit status).",
		       pid, WEXITSTATUS(status));
		break;
	case 19:
		syslog(LOG_ERR, "PPPD[%d] exited with code %d: "
		       "We failed to authenticate ourselves to the peer.",
		       pid, WEXITSTATUS(status));
		break;
	default:
		syslog(LOG_ERR, "PPPD[%d] exited with an unrecognized "
		       "exit code", pid);
	}
}
