%{
#include <stdio.h>
#include <string.h>
#include "options.h"

#define YYSTYPE char *

int yydebug=0;
extern char *yytext;
char *ip_address;

void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
        fprintf(stderr,"error: %s\n",yytext);

}

int yywrap()
{
        return 1;
}

%}
%token NUMBER QUOTE FILENAME PHONE DEVICE IP_CMD IPNUMBER COMMA
%token TUN_DEV LOCAL_DEV NETMASK_CMD PPPD DEBUG_CMD LIST
%%

commands:
        |        
        commands command
        ;

command: phone_cmd
	| device_cmd
	| ip_cmd
	| netmask_cmd
	| debug_cmd
	| list_cmd
        ;


quotedname:
        QUOTE FILENAME QUOTE
        {
                $$=$2;
        }
        ;


quotedname_list: quotedname  
	        { printf("bname:%s\n",$1); }
	| quotedname_list COMMA quotedname 
	  { printf("aname:%s\n",$1); }
 	;

list_cmd: LIST quotedname_list
	{
	printf("list cmd found <%s>\n",yytext);
	}
	;

number:
	NUMBER
	{
	$$=$1;
	}
	;

debug_cmd:
	DEBUG_CMD number
	{
	  debug=$2;
	  printf("debug number %s\n",$2);
	}
	;
	
device_cmd:
	DEVICE quotedname
	{
	  printf("Device name %s\n",$2);
	}
	;

device_cmd:
	PPPD quotedname
	{
	  path_pppd=$2;
	  printf("PPPD path %s\n",$2);
	}
	;

ip_cmd:
	IP_CMD TUN_DEV IPNUMBER
        {
		tun_ip=ip_address;
		printf("tun IP_address %s\n",ip_address);
	}
	| IP_CMD LOCAL_DEV IPNUMBER
	{
		local_ip=ip_address;
		printf("local IP_address %s\n",ip_address);
	}
	;

netmask_cmd:  
	NETMASK_CMD TUN_DEV IPNUMBER
	{
	netmask_tun = ip_address;
	printf("tun netmask %s\n",ip_address);
	}
	| NETMASK_CMD LOCAL_DEV IPNUMBER
	{
	netmask_ppp = ip_address;
	printf("local netmask  %s\n",ip_address);
	}
	;



phone_cmd:
	PHONE quotedname
	{
	  path_phoneconfig=$2;
	  printf("Phone number file %s\n",$2);
	}
	; 
