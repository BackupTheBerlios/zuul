#include <signal.h>
#include <stdlib.h>
#include "list.h"

void ppp_start(struct device_list *);
void ppp_return_code_resolve(int status, pid_t pid);
