/* zuul.c
 * 
 * Copyright (c) 2004 Peter Gerbrandt
 *
 * based on diald.c from the diald project
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
#include "zuul.h"
#include "list.h"
#include "ppp.h"
#include "options.h"

static char *RCSID="$Id: zuul.c,v 1.1 2004/03/05 00:10:51 systor Exp $";

/* init some options */
void init_opts(void)
{
	path_pppd="/usr/sbin/pppd";
	path_phoneconfig="/etc/mmdiald/phone.conf";
	modem = 1;
	crtscts = 1;
	mtu = 1500;
	metric = 0;
	path_ifconfig = "/sbin/ifconfig";
	path_route = "/sbin/route";
	local_ip="10.99.0.1";
	tun_ip="10.99.0.1";
	net_ip="10.99.0.0";
	netmask_tun="255.255.255.0";
	net_route=1;
	debug=0x0080;
}

/* resolve ip to telephone number */
char *iptotel(char *address)
{
	int i=0;

	char *ip[40];
	char *tel[40];
	
	IPTEL(0, "10.99.0.101", "21");
	IPTEL(1, "10.99.0.102", "22");
	IPTEL(2, "10.99.0.103", "23");
	IPTEL(3, "10.99.0.104", "24");
        IPTEL(4, "10.99.0.105", "25");
        IPTEL(5, "10.99.0.106", "26");
        IPTEL(6, "10.99.0.107", "27");
        IPTEL(7, "10.99.0.108", "28");
        IPTEL(8, "10.99.0.109", "29");

	for(i=0; i<9; ++i)
	{
		if(inet_addr(ip[i]) == inet_addr(address)){
			if(debug&DEBUG_IPTOTEL)
				syslog(LOG_DEBUG, "IPTOTEL ip: %s  tel: %s\n",
				       ip[i], tel[i]);
			return tel[i];
		}
	}
	return NULL;
}

int main()
{
	char buf[1600], *infile;
	int ret, f1, l, fm, already_connected=0;
	fd_set fds;
	struct ifreq ifr;
	struct ip *ip;
	struct in_addr address;
	struct device_list entry, *np, *ce;
	struct sigaction sa;
	extern FILE *yyin;

	

	if(debug){
		char *outname;
		extern FILE *yyout;
		outname = "mmdiald.out";
		yyout = fopen(outname, "w");
		if(yyout == NULL){
			syslog(LOG_ERR, "cannot open out file %s\n", outname);
			exit(EXIT_FAILURE);
		}
	}
			

	infile = "zuul.conf";
	
	yyin = fopen(infile, "r");
	if(yyin == NULL){
		syslog(LOG_ERR, "cannot open config file %s\n", infile);
		exit(EXIT_FAILURE);
	}

	init_opts();

	yyparse();



	memset(&sa, 0, sizeof(sa));
//	sa.sa_mask = sig_mask;
	sa.sa_flags = 0;
	SIGNAL(SIGCHLD, sig_chld);
	strncpy(ifr.ifr_name, "tun%d", IFNAMSIZ);
	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN;
	
	f1 = open("/dev/net/tun", O_RDWR);
	if(f1 < 0){
		syslog(LOG_ERR, "open(): /dev/net/tun: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	/* init modem devices */
	entry.device = "ttyS0";
	entry.config = NULL;
	entry.link_pid = 0;
	entry.dial_pid = 0;
	entry.timeout = 0;
	address.s_addr = inet_addr("10.99.0.1");
	entry.laddr = &address;
	entry.raddr = NULL;
	entry.status = 0;
	add_dl_entry(&entry);

	entry.device = "ttyS1";
	address.s_addr = inet_addr("10.99.0.1");
	entry.laddr = &address;
	add_dl_entry(&entry);
	
	fm = f1 + 1;
	// TUNSETNOCSUM
	ret = ioctl(f1, TUNSETIFF, (void *) &ifr);
	if(ret < 0){
		close(f1);
		exit(EXIT_FAILURE);
	}
	
	iface_start("proxy", ifr.ifr_name, local_ip, NULL, NULL);

	syslog(LOG_INFO, "listening on device %s\n", ifr.ifr_name);
	
	FD_ZERO(&fds);
	FD_SET(f1, &fds);
	
	while(1){
		select(fm, &fds, NULL, NULL, NULL);
		
		if(!FD_ISSET(f1, &fds) ) {
			continue;
		}
		memset(buf, 0, sizeof(buf));
		l = read(f1,buf,sizeof(buf));

		/* read nothing */
		if(!l)
			continue;

		if(sizeof(struct ip) == strlen(buf))
			ip = (struct ip *)buf;
		else
			ip = buf + 4;

		sprintf(buf, "%s", inet_ntoa(ip->ip_dst));

		if(debug&DEBUG_PACKETS)
			syslog(LOG_DEBUG, "PACKET From: %s To: %s\n",
			       inet_ntoa(ip->ip_src), buf);

		/* drop if not resolveable */
		if(iptotel(inet_ntoa(ip->ip_dst)) == NULL)
		   continue;
		ce = NULL;
		already_connected = 0;

		for (np = dl_head.lh_first; np != NULL; 
		     np = np->node.le_next)
		{

			/* no device in this entry -> next */
			if(np->device == NULL)
				continue;
			
			/* we need to check all devices for a
			   connection to the current target */
			if(np->raddr != NULL &&
			   (strcmp(inet_ntoa(ip->ip_dst), 
			       inet_ntoa(*np->raddr)) == 0))
			{
				already_connected = 1;
				continue;
			}

			/* is not ready to make a new connection yet -> next*/
			if(np->status != 0)
				continue;
			
			ce = np;
		}
		/* no free device */
		if(ce == NULL){
			continue;
		}

		/* remote address already in use */
		if(already_connected){
			continue;
		}
		syslog(LOG_INFO, 
		       "establishing connection to %s\n",
		       inet_ntoa(ip->ip_dst));
		ce->raddr = malloc(sizeof(struct in_addr));
		assert(ce->raddr != NULL);
		memcpy(ce->raddr, &ip->ip_dst, sizeof(struct in_addr));
		ppp_start(ce);
	}
	return(0);
}
