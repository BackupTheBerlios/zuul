/* A Bison parser, made from gram.tab.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef BISON_Y_TAB_H
# define BISON_Y_TAB_H

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     NUMBER = 258,
     QUOTE = 259,
     FILENAME = 260,
     PHONE = 261,
     DEVICE = 262,
     IP_CMD = 263,
     IPNUMBER = 264,
     COMMA = 265,
     TUN_DEV = 266,
     LOCAL_DEV = 267,
     NETMASK_CMD = 268,
     PPPD = 269,
     DEBUG_CMD = 270,
     LIST = 271
   };
#endif
#define NUMBER 258
#define QUOTE 259
#define FILENAME 260
#define PHONE 261
#define DEVICE 262
#define IP_CMD 263
#define IPNUMBER 264
#define COMMA 265
#define TUN_DEV 266
#define LOCAL_DEV 267
#define NETMASK_CMD 268
#define PPPD 269
#define DEBUG_CMD 270
#define LIST 271




#ifndef YYSTYPE
typedef int yystype;
# define YYSTYPE yystype
#endif

extern YYSTYPE yylval;


#endif /* not BISON_Y_TAB_H */

