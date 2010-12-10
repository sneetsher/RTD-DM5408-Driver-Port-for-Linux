#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../include/dm5408_lib.h"


int foo(void){return -1;};

uchar read_status(void)
{
 //return(inp(BaseAddress + r_STATUS_5408));
 return 0;
};
