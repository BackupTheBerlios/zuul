int pppd_argc;
char **pppd_argv;
int modem_fd;
unsigned long local_addr;
char *path_phoneconfig;
char *path_pppd;
int modem;
int crtscts;
int mtu;
int mru;
char *remote_ip;
char *local_ip;
char *tun_ip;
int metric;
//proxy_t proxy;
char *path_route;
char *path_ifconfig;
char *net_ip;
int debug;
char *path_ip;
char *path_route;
#define SHELL_WAIT      1
int net_route;
char *ifsetup;
char *path_ifconfig;
int mtu;

int debug;
char *current_dev;              /* name of the current device */
char *link_name;
#define SHELL_WAIT      1
int running_pid;                /* current system command pid */
int running_status;             /* status of last system command */

char *netmask_tun;
char *netmask_ppp;
#define SHELL_NOWAIT    0


/* Define DEBUG flags */
#define DEBUG_FILTER_MATCH      0x0001
#define DEBUG_PROXYARP          0x0004
#define DEBUG_VERBOSE           0x0008
#define DEBUG_STATE_CONTROL     0x0010
#define DEBUG_TICK              0x0020
#define DEBUG_CONNECTION_QUEUE  0x0040

