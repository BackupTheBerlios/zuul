%{
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
extern char *ip_address;
%}
%%
phone                   return PHONE;
device                  return DEVICE;
pppd                    return PPPD;
ip                      return IP_CMD;
debug                   return DEBUG_CMD;
netmask                 return NETMASK_CMD;
tun                     return TUN_DEV;
ppp                     return LOCAL_DEV;
[0-9]+                  yylval=strdup(yytext); return NUMBER;
\"                      return QUOTE;
\,                      return COMMA;
[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}  ip_address=strdup(yytext); return IPNUMBER;
[a-zA-Z0-9\/.-]+        yylval=strdup(yytext); return FILENAME;
\n                      /* ignore end of line */;
[ \t]+                  /* ignore whitespace */;
#.*                     /* ignore commentar */;
%%
