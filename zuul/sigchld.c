/* sigchld.c
 * 
 * Copyright (c) 2004 Peter Gerbrandt
 *
 * based on sigchld.c from the diald project
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

#include <signal.h>
#include <sys/wait.h>
#include "list.h"
#include "zuul.h"

static char *RCSID="$Id: sigchld.c,v 1.1 2004/03/05 00:10:51 systor Exp $";

void ppp_return_code_resolve(int status, pid_t pid);

int dial_pid = 0;
int running_pid = 0;
int dial_status = 0;
int running_status = 0;
//static sigset_t sig_mask;

int active_child=0;
//char *path_pppd="./test-sh.sh";
char *path_pppd="/usr/sbin/pppd";

#define SIGNAL(s, handler)      { \
        sa.sa_handler = handler; \
        if (sigaction(s, &sa, NULL) < 0) { \
            syslog(LOG_ERR, "sigaction(%d): %m", s); \
            exit(1); \
        } \
    }

/*
 * A child process died. Find out which one.
 */
void sig_chld(int sig)
{
	pid_t pid;
	int status;
	struct device_list *dl_n1;
	static int seq = 0;
	++seq;
	while ((pid = waitpid(-1,&status,WNOHANG)) > 0) {
		if (1)//(debug&DEBUG_VERBOSE)
			syslog( LOG_DEBUG, "SIGCHLD[%d]: pid %d %s, status %d",
				seq, pid,
				dl_find_by_link_pid(pid) != NULL ? "link"
				: pid == dial_pid ? "dial"
				: pid == running_pid ? "system"
				: "other",
				status);
		dl_n1 = dl_find_by_link_pid(pid);
		if (dl_n1 != NULL)
		{
			dl_n1->status = 0;
			dl_n1->link_pid = 0;
			free(dl_n1->raddr);
			dl_n1->raddr = NULL;
			ppp_return_code_resolve(status,pid);
		}
		
		else if (!WIFEXITED(status))
			syslog(LOG_ERR,"Abnormal exit (status %d) on pid %d",
			       status,pid);
		else if (WEXITSTATUS(status) != 0)
			syslog(LOG_ERR,"Nonzero exit status (%d) on pid %d",
			       WEXITSTATUS(status),pid);
		if (pid > 0) {
			if (WIFSIGNALED(status)) {
				syslog(LOG_WARNING, "child process %d "
				       "terminated with signal %d",
				       pid, WTERMSIG(status));
			}
		}
	}
	if (pid && errno != ECHILD)
		syslog(LOG_ERR, "waitpid: %m");
	return;
}

