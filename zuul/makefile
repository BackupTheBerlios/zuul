CC=gcc
CFLAGS=-g -I. -W -Wall -pipe -Wno-unused
YACC=bison
LEX=flex
YFLAGS=-y -d

zuul: zuul.c list.c ppp.c route.c shell.c sigchld.c gram.tab.c parse.c

parse.c: parse.l

gram.tab.c: gram.tab.y

clean: 
	rm -f *.o zuul parse.c gram.tab.c *~
