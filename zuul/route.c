/* route.c
 * 
 * Copyright (c) 2004 Peter Gerbrandt
 *
 * based on route.c from the diald project
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

#include <sys/types.h>
#include "zuul.h"
#include "options.h"

static char *RCSID="$Id: route.c,v 1.1 2004/03/05 00:10:51 systor Exp $";

static void
add_routes(char *desc, char *iface, char *lip, char *rip, int metric)
{
	char win[32];
	char buf[1024];
	
	/* FIXME: Should not report an error if the error was just that the
	 *        route we tried to add already exists.
	 *        (A new error reported by more recent kernels.)
	 */
	
	if (debug&DEBUG_VERBOSE)
		syslog(LOG_DEBUG, "%s: Establishing routes for %s", 
		       desc, iface);
	
#if 1
    /* FIXME: this is only needed for 2.0 kernels. 2.2 and beyond
     * create routes automatically when the interface is configured.
     * On 2.2 and later kernels this just creates some annoying
     * duplicate routes. But if the metric is non-zero we can,
     * and should, get rid of the original zero metric route.
     */
	if (rip) {
		if (path_ip && *path_ip) {
			sprintf(buf,
				"%s route replace %s dev %s scope"
				" link%s%s metric %d %s",
				path_ip, rip, iface,
				lip ? " src " : "",
				lip ? lip : "", metric, win); 
		} else {
			sprintf(buf,"%s add %s metric %d %s dev %s",
				path_route, rip, metric, win, iface);
		}
		run_shell(SHELL_WAIT, desc, buf, -1);
		
		if (metric) {
			if (path_ip && *path_ip) {
				sprintf(buf,"%s route del %s dev %s scope"
					" link%s%s metric 0 %s",
					path_ip, rip, iface,
					lip ? " src " : "",
					lip ? lip : "", win); 
			} else {
				sprintf(buf,"%s del %s metric 0 %s dev %s",
					path_route, rip, win, iface); 
			}
			run_shell(SHELL_WAIT, desc, buf, -1);
		}
	}
#endif
	
	/* Add in a default route for the link if required. */
	if (net_route) {
		if (path_ip && *path_ip) {
			sprintf(buf, "%s route replace default dev %s scope"
				" link%s%s metric %d %s",
				path_ip, iface,
				lip ? " src " : "",
				lip ? lip : "", metric, win); 
		} else {
			sprintf(buf,"%s add default metric %d %s netmask %s"
				" dev %s", path_route, 
				metric, win, netmask_ppp, iface);
		}
		run_shell(SHELL_WAIT, desc, buf, -1);
	}
}

void
iface_start(char *mode, char *iftype, char *lip, char *rip, char *bip)
{
	char *iface, desc[32], buf[1024];
	
	strcpy(desc, "start ");
	snprintf(desc+6, sizeof(desc)-6-1, "%s", iftype);
	iface = desc+6;
	
	/* mark the interface as up */
	if (ifsetup) {
		sprintf(buf, "%s start %s %s",
			ifsetup, mode, iface);
		run_shell(SHELL_WAIT, desc, buf, -1);
		return;
	}
	
	/* With no ifsetup script we have to do it all ourselves. */
	if (lip) {
		sprintf(buf,"%s %s %s%s%s%s%s netmask %s mtu %d up",
			path_ifconfig, iface, lip,
			rip ? " pointopoint " : "",
			rip ? rip : "",
			bip ? " broadcast " : "",
			bip ? bip : "",
			netmask_tun ? netmask_tun : "255.255.255.255", mtu);
		run_shell(SHELL_WAIT, desc, buf, -1);
	}
	
	add_routes(desc, iface, lip, net_ip, metric);
}

void
iface_down(char *mode, char *iftype, int ifunit,
	   char *lip, char *rip, char *bip, int metric)
{
	char *iface, desc[32], buf[128];
	
	strcpy(desc, "down ");
	snprintf(desc+5, sizeof(desc)-5-1, "%s%d", iftype, ifunit);
	iface = desc+5;
	
	sprintf(buf, "%s %s down",
		path_ifconfig, iface
		);
	run_shell(SHELL_WAIT, desc, buf, -1);
}
