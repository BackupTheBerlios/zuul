/* shell.c
 * 
 * Copyright (c) 2004 Peter Gerbrandt
 *
 * based on shell.c from the diald project
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

#include <sys/stat.h>
#include "zuul.h"
#include "options.h"

static char *RCSID="$Id: shell.c,v 1.1 2004/03/05 00:10:51 systor Exp $";

typedef struct pipe {
	struct pipe *next;
	char *name;         /* the name given to this channel */
	int access;         /* access flags */
	int fd;             /* file descriptor */
	int count;          /* number of characters in the buffer now */
	char buf[1024];
} PIPE;

int
run_shell(int mode, const char *name, const char *buf, int d)
{
	int d2, p[2];
	pid_t pid;
	FILE *fd;

	if (debug&DEBUG_VERBOSE)
		syslog(LOG_DEBUG,"running '%s'",buf);
	
	if (pipe(p))
		p[0] = p[1] = -1;
	
	pid = fork();
	
	if (pid < 0) {
		syslog(LOG_ERR, "failed to fork and run '%s': %m",buf);
		return -1;
	}

	if (pid == 0) {
		if (d >= 0) {
			/* Run in a new process group and foreground ourselves
			 * on the tty (SIGTTOU is ignored).
			 * N.B. If we are in dev mode we have /dev/null and
			 * not a serial line...
			 */
			setpgrp();
			if (tcsetpgrp(d, getpid()) < 0 && errno != ENOTTY)
				syslog(LOG_ERR,"dial: failed to set pgrp: %m");
		} else {
			setsid();    /* No controlling tty. */
			umask (S_IRWXG|S_IRWXO);
			chdir ("/"); /* no current directory. */
		}


		/* make sure the stdin, stdout and stderr get directed 
		   to /dev/null */
		if (p[0] >= 0) close(p[0]);
		d2 = open("/dev/null", O_RDWR);
		if (d >= 0) {
			if (p[1] != 2) dup2((p[1] >= 0 ? p[1] : d2), 2);
			close(d2);
			if (d != 0) {
				dup2(d, 0);
				close(d);
			} else {
				fcntl(d, F_SETFD, 0);
			}
			dup2(0, 1);
		} else {
			if (d2 != 0) {
				dup2(d2, 0);
				close(d2);
			}
			if (p[1] != 1) dup2((p[1] >= 0 ? p[1] : 0), 1);
			if (p[1] != 2) dup2((p[1] >= 0 ? p[1] : 0), 2);
		}
		if (p[1] > 2) close(p[1]);

		/* set the current device (compat) */
		if (current_dev)
			setenv("MODEM", current_dev, 1);

		if (current_dev)
			setenv("DIALD_DEVICE", current_dev, 1);
		if (link_name)
			setenv("DIALD_LINK", link_name, 1);

		execl("/bin/sh", "sh", "-c", buf, (char *)0);
		syslog(LOG_ERR, "could not exec /bin/sh: %m");
		_exit(127);
		/* NOTREACHED */
	}

	if (p[1] >= 0) close(p[1]);

	if (mode & SHELL_WAIT) {
		running_pid = pid;

		if (p[0] >= 0 && (fd = fdopen(p[0], "r"))) {
			char buf[1024];

			while (fgets(buf, sizeof(buf)-1, fd)) {
				buf[sizeof(buf)-1] = '\0';
				syslog(LOG_INFO, "%s: %s", name, buf);
			}

			fclose(fd);
		}

		return running_status;
	}
	return pid;
}


void
run_state_script(char *name, char *script, int background)
{
	char buf[1024];

	snprintf(buf, sizeof(buf)-1, "%s '%s' '%s' '%s'",
		 script,
		 netmask_ppp ? netmask_ppp : "255.255.255.255",
		 local_ip,
		 remote_ip);
	buf[sizeof(buf)-1] = '\0';

	if (debug&DEBUG_VERBOSE)
		syslog(LOG_INFO,"running %s script '%s'", name, buf);

	if (background)
		running_pid = run_shell(SHELL_NOWAIT, name, buf, -1);
	else
		run_shell(SHELL_WAIT, name, buf, -1);
}
