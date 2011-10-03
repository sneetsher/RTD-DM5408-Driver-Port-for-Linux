/*******************************************************************************

    RTD DM5408 Acquisition Card Driver
    Copyright (C) 2010  Abdellah Chelli <abdellahchelli@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <ncurses.h>


#include "libdm5408.h"

int main(int argc, char *argv[])
{
  int file_desc;
  uchar test = 0;

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
    printf("Device file: %s opened.\n", dev_name);

  test = read_status_register(file_desc);
  printf("Status reg: 0x%02X.\n", test);

  init_board(file_desc);
  set_port0_direction_5812(file_desc, 0, 0);
  set_port1_direction_5812(file_desc, 0, 0);

  test = read_status_register(file_desc);
  printf("Status reg: 0x%02X.\n", test);

  test = read_dio_5812(file_desc, 0, 0);
  printf("Digital port0: 0x%02X.\n", test);

  test = read_dio_5812(file_desc, 0, 1);
  printf("Digital port1: 0x%02X.\n", test);

  // Closing dev file
  close(file_desc);
  printf("Device file: %s closed.\n", dev_name);
  return 0;
}
