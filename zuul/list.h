/* list.h
 * 
 * Copyright (c) 2004 Walter Harms
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
#ifndef _LIST_H_
#define _LIST_H_ v0.0
#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct device_list {
	LIST_ENTRY(device_list) node;
	char *config;
	char *device;
	pid_t  link_pid;
	pid_t  dial_pid;
	time_t timeout;
	struct in_addr *laddr;
	struct in_addr *raddr;
	int status;
} ;

extern int add_dl_entry(struct device_list *entry);
extern struct device_list *dl_find_by_link_pid(pid_t pid);
extern int dl_remove_by_link_pid(pid_t pid);
extern int dump_dl_list();

LIST_HEAD(dl_listhead, device_list) dl_head;
struct dl_listhead *dl_headp; 

#endif
