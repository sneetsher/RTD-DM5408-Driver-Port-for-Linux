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
#include <signal.h>
#include <time.h>

#include "libdm5408.h"

static void finish(int);

int main(int argc, char *argv[])
{
  int file_desc;
  uchar test = 0;
  bool cont = TRUE;
  int count = 0;

  char dev_name[] =  DEV_NAME;
  strcat(dev_name, "0");

  /* initialize your non-curses data structures here */
  (void) signal(SIGINT, finish);   /* arrange interrupts to terminate */

  (void) initscr();     /* initialize the curses library */
  keypad(stdscr, TRUE); /* enable keyboard mapping */
  (void) nonl();        /* tell curses not to do NL->CR/NL on output */
  (void) cbreak();      /* take input chars one at a time, no wait for \n */
  (void) echo();        /* echo input - in color */

  if (has_colors())
  {
    start_color();

    /*
     * Simple color assignment, often all we need.  Color pair 0 cannot
     * be redefined.  This example uses the same value for the color
     * pair as for the foreground color, though of course that is not
     * necessary:
     */
    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);
    init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(4, COLOR_BLUE,    COLOR_BLACK);
    init_pair(5, COLOR_CYAN,    COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_WHITE,   COLOR_BLACK);
  }

  while (cont)
  {
    attrset(COLOR_PAIR(7));
    mvaddch(0,0,'>');
    mvprintw(0,5,"cmd #: %4d",count);
    attrset(COLOR_PAIR(2));
    int c = mvgetch(0,1);/* refresh, accept single keystroke of input */
    count++;

    /* process the command keystroke */
    switch (c)
    {

      case 'o':       /* Open dev 0 0x0300h */
        file_desc = open( dev_name, 0);
        if (file_desc < 0)
        {
          attrset(COLOR_PAIR(1));
          mvprintw(1,0,"Can't open device file: %s.\n", dev_name);
        }
        else
        {
          attrset(COLOR_PAIR(7));
          mvprintw(1,0,"Device file: %s opened.",dev_name);
          init_board(file_desc);
        };
        break;

      case 'c':       /* Close dev 0 0x0300h */
        close(file_desc);
        attrset(COLOR_PAIR(7));
        mvprintw(1,0,"Device file: %s closed.\n", dev_name);
        break;

      case 's':
        test = read_status_register(file_desc);
        attrset(COLOR_PAIR(3));
        mvprintw(3,0,"Status Reg: 0x%02X.\n", test);
        break;

      case 'd':       /* Config both digital ports for input */
        set_port0_direction_5812(file_desc, 0, 0);
        set_port1_direction_5812(file_desc, 0, 0);
        break;

      case 'r':       /* Read both digital ports */
        attrset(COLOR_PAIR(3));
        test = read_dio_5812(file_desc, 0, 0);
        mvprintw(5,0,"Digital Port 0: 0x%02X.\n", test);
        test = read_dio_5812(file_desc, 0, 1);
        mvprintw(6,0,"Digital Port 1: 0x%02X.\n", test);
        break;

      case 'x':       /* Exit */
        cont = FALSE;
        break;
    }
  }

  sleep(1);
  finish(0);          /* we're done */
  return 0;
}


static void finish(int sig)
{
  endwin();
  /* do your non-curses wrapup here */

  exit(0);
}
