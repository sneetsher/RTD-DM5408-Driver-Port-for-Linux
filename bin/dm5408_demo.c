#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/dm5408_lib.h"

int main(int argc, char *argv[])
{
    int file_desc;

    // Lib Testing
    int c=foo();
    printf("foo() test!-1 = %d.\n", c);

    // Opening dev file
    char dev_name[] =  DEV_NAME;
    strcat(dev_name, "0");
    file_desc = open( dev_name, 0);
    if (file_desc < 0)
    {
        printf("Can't open device file: %s.\n", dev_name);
        exit(-1);
    }
    else
        printf("Device file: %s opened with %d as a descriptor.\n", dev_name, file_desc);

    // Closing dev file
    close(file_desc);
    printf("Device file: %s closed.\n", dev_name, file_desc);
    return 0;
}
