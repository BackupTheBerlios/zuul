/* list.c
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
#include "list.h"

//extern LIST_HEAD(dl_listhead, device_list) dl_head;
//extern struct dl_listhead *dl_headp; 

int add_dl_entry(struct device_list *entry)
{
	struct device_list *n1;
	n1 = malloc(sizeof(struct device_list ));
	if (!n1) {
		fprintf(stderr,"cant alloc in %s\n",__func__);
		exit(EXIT_FAILURE);
	}
	memcpy(n1,entry,sizeof(struct device_list));

	LIST_INSERT_HEAD(&dl_head, n1, node);
	return 0;
}

struct device_list *dl_find_by_link_pid(pid_t pid)
{
	struct device_list *np;
	for (np = dl_head.lh_first; np != NULL; np = np->node.le_next)
		if (np->link_pid==pid)	return np;

	return NULL;		/* not found */
}

int dl_remove_by_link_pid(pid_t pid)
{
	struct device_list *np=dl_find_by_link_pid(pid);
	if (!np)
		return -1;	/* not found */	
	LIST_REMOVE(np,node);
	return 0;
}


int dump_dl_list() {
	struct device_list *np;
	for (np = dl_head.lh_first; np != NULL; np = np->node.le_next) {
		printf("device=%s ",np->device);
		printf("link=%d\n",np->link_pid);
	}
	return 0;
}

