/* zuul.h
 * 
 * Copyright (c) 2004 Peter Gerbrandt
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
#ifndef _ZUUL_H
#define _ZUUL_H v0.0

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_tun.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

/* Define DEBUG flags */
#define DEBUG_FILTER_MATCH      0x0001
#define DEBUG_PROXYARP          0x0004
#define DEBUG_VERBOSE           0x0008
#define DEBUG_STATE_CONTROL     0x0010
#define DEBUG_TICK              0x0020
#define DEBUG_CONNECTION_QUEUE  0x0040
#define DEBUG_PACKETS           0x0080
#define DEBUG_IPTOTEL           0x0100


int ip_decode(char *);
char *iptotel(char *);
void sig_chld(int sig);
int run_shell(int mode, const char *name, const char *buf, int d);
void iface_start(char *mode, char *iftype, char *lip, char *rip, char *bip);


#define IPTEL(idn, ipn, teln) ip[idn] = ipn; tel[idn] = teln;
#define SIGNAL(s, handler)      { \
        sa.sa_handler = handler; \
        if (sigaction(s, &sa, NULL) < 0) { \
            syslog(LOG_ERR, "sigaction(%d): %m", s); \
            exit(1); \
        } \
    }

#endif
