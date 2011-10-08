/*******************************************************************************

    RTD DM5408 Acquisition Card Driver Port for Linux
    Copyright (C) 2010-2011  Abdellah Chelli <abdellahchelli@gmail.com>

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

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libdm5408.h"

/* drvr5408.h *********************************************************/

void
init_board(int file_desc)
{
  clear_board(file_desc);
  clear_ad_dma_done(file_desc);
  clear_irq(file_desc);
  clear_channel_gain_table(file_desc);
  clear_ad_fifo(file_desc);
}

// BA+0 R
uchar
read_status_register(int file_desc)
{
  struct dm5408_io8 arg;

  arg.reg = REG_STATUS;
  arg.val = 0;

  ioctl(file_desc, DM5408_IOC_INB, &arg);

  return arg.val;
}

uchar
is_adc_fifo_empty(uchar status)
{
  return !(status & ADC_FIFO_EMPTY);
}

uchar
is_adc_fifo_full(uchar status)
{
  return (status & ADC_FIFO_FULL);
}

uchar
is_adc_halted(uchar status)
{
  return (status & ADC_HALTED);
}

uchar
is_adc_converting(uchar status)
{
  return (status & ADC_CONVERTING);
}

uchar
is_pacer_clock_on(uchar status)
{
  return (status & PACER_CLOCK_ON);
}

uchar
is_adc_dma_done(uchar status)
{
  return (status & ADC_DMA_DONE);
}

uchar
is_irq(uchar status)
{
  return (status & IRQ);
}

uchar
is_digital_irq(uchar status)
{
  return (status & DIGITAL_IRQ);
}

// BA+0 W
void
write_control_register(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CONTROL;
  arg.val = value;

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

// BA+2 W
void
select_register(int file_desc, enum select_reg_val value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CONTROL;
  arg.val = value;
  arg.mask = 0xFC;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

// BA+0 W
void
select_timer_counter(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CONTROL;
  arg.val = value? 0x04: 0x00;
  arg.mask = 0x04;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

// BA+0 W
void
enable_channel_gain_data_store(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CONTROL;
  arg.val = value? 0x08: 0x00;
  arg.mask = 0x08;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

// BA+0 W
void
enable_tables(int file_desc, uchar ad_value, uchar dg_value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CONTROL;
  arg.val = (ad_value? 0x04: 0x00) | (ad_value? 0x08: 0x00);
  arg.mask = 0xC0;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

// BA+1 R
void
start_conversion(int file_desc)
{
  struct dm5408_io8 arg;

  arg.reg = REG_START_CONVERSION;

  ioctl(file_desc, DM5408_IOC_INB, &arg);
}

// BA+1 W
void
set_channel_gain(int file_desc, uchar channel, uchar gain)
{
  struct dm5408_io8 arg;

  arg.reg = REG_GAIN;
  arg.val = channel | (gain << 4);

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

//
void
load_ad_table(int file_desc, uchar entry_nb, ad_table_row_t *ad_table)
{
  struct dm5408_io8 arg;
  uchar i;

  // enable ad table loading
  arg.reg = REG_CONTROL;
  arg.val = 0x10;
  arg.mask = 0x30;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);

  // filling ad table
  arg.reg = REG_AD_TABLE;
  for (i = 0; i < entry_nb; i++)
  {
    arg.val = ad_table[i].channel | (ad_table[i].gain << 4) | (ad_table[i].skip << 7);

    ioctl(file_desc, DM5408_IOC_OUTB, &arg);
  }

  // disable ad table loading
  arg.reg = REG_CONTROL;
  arg.val = 0x00;
  arg.mask = 0x30;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

//
void
load_digital_table(int file_desc, uchar entry_nb, uchar *digital_table)
{
  struct dm5408_io8 arg;
  uchar i;

  // enable digital table loading
  arg.reg = REG_CONTROL;
  arg.val = 0x20;
  arg.mask = 0x30;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);

  // filling digital table
  arg.reg = REG_DIGITAL_TABLE;
  for (i = 0; i < entry_nb; i++)
  {
    arg.val = digital_table[i];

    ioctl(file_desc, DM5408_IOC_OUTB, &arg);
  }

  // disable digital table loading
  arg.reg = REG_CONTROL;
  arg.val = 0x00;
  arg.mask = 0x30;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

short
read_ad_data(int file_desc)
{
  struct dm5408_io16 arg;

  arg.reg = REG_ADC_LSB;
  arg.val = 0x0000;

  ioctl(file_desc, DM5408_IOC_INW, &arg);

  return (short) (arg.val >> 4);
}

ushort
read_channel_gain_data_store(int file_desc)
{
  struct dm5408_io16 arg;

  arg.reg = REG_ADC_LSB;
  arg.val = 0x0000;

  ioctl(file_desc, DM5408_IOC_INW, &arg);

  return arg.val;
}

short
read_ad_data_with_marker(int file_desc)
{
  struct dm5408_io16 arg;

  arg.reg = REG_ADC_LSB;
  arg.val = 0x0000;

  ioctl(file_desc, DM5408_IOC_INW, &arg);

  return (short) arg.val;
}

void
load_trigger0_register(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value;

  select_register(file_desc, 0);

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
load_trigger1_register(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value;

  select_register(file_desc, 1);

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
load_irq_register(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_IRQS;
  arg.val = value;

  select_register(file_desc, 2);

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
set_conversion_select(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value;
  arg.mask = 0x03;

  select_register(file_desc, 0);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_start_trigger(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value << 2;
  arg.mask = 0x1C;

  select_register(file_desc, 0);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_stop_trigger(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value << 5;
  arg.mask = 0xE0;

  select_register(file_desc, 0);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_pacer_clock_source(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value;
  arg.mask = 0x01;

  select_register(file_desc, 1);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_burst_trigger(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value << 1;
  arg.mask = 0x06;

  select_register(file_desc, 1);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_trigger_polarity(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value << 3;
  arg.mask = 0x08;

  select_register(file_desc, 1);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_trigger_repeat(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value << 4;
  arg.mask = 0x10;

  select_register(file_desc, 1);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_sample_counter_stop(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.val = value << 5;
  arg.mask = 0x20;

  select_register(file_desc, 1);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_irq_source(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_IRQS;
  arg.val = value;
  arg.mask = 0x07;

  select_register(file_desc, 2);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
enable_irq(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_IRQS;
  arg.val = value << 3;
  arg.mask = 0x08;

  select_register(file_desc, 2);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
enable_irq_sharing(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_IRQS;
  arg.val = value << 4;
  arg.mask = 0x10;

  select_register(file_desc, 2);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
digital_irq_mask(int file_desc, uchar value)
{
  struct dm5408_io8 arg;

  arg.reg = REG_IRQS;
  arg.val = value << 5;
  arg.mask = 0x20;

  select_register(file_desc, 2);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
clear_ad_fifo(int file_desc)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CLEAR;
  arg.val = 0x00;

  select_register(file_desc, 0);

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
clear_channel_gain_table(int file_desc)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CLEAR;
  arg.val = 0x00;

  select_register(file_desc, 1);

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
reset_channel_gain_table(int file_desc)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CLEAR;
  arg.val = 0x00;

  select_register(file_desc, 2);

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
clear_board(int file_desc)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CLEAR;
  arg.val = 0x00;

  select_register(file_desc, 3);

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
clear_irq(int file_desc)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CLEAR_SOFT_IRQ;

  ioctl(file_desc, DM5408_IOC_INB, &arg);
}

void
clear_ad_dma_done(int file_desc)
{
  struct dm5408_io8 arg;

  arg.reg = REG_CLEAR_DMA_FLAG;

  ioctl(file_desc, DM5408_IOC_INB, &arg);
}

void
load_dac(int file_desc, uchar port, ushort value)
{
  struct dm5408_io16 arg;

  arg.reg = port? REG_DAC2_LSB : REG_DAC1_LSB;
  arg.val = value;

  ioctl(file_desc, DM5408_IOC_OUTW, &arg);
}


void
update_dacs(int file_desc)
{
  struct dm5408_io8 arg;

  arg.reg = REG_UPDATE_DACS;

  ioctl(file_desc, DM5408_IOC_INB, &arg);
}


void
clock_mode(int file_desc, uchar counter, uchar mode)
{
  struct dm5408_io8 arg;

  arg.reg = REG_COUNTER_CONTROL;
  arg.val = (counter << 6) + (mode << 1) + 0x03;

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
clock_divisor(int file_desc, uchar counter, ushort divisor)
{
  struct dm5408_io8 arg;

  arg.reg = REG_COUNTER0 + counter;
  arg.val = divisor & 0xFF; // LSB

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);

  arg.val = (divisor & 0xFF00) >> 8; //MSB

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}


float
set_pacer_clock(int file_desc ,float clock_rate)
{
  struct dm5408_io8 arg;

  arg.reg = REG_TRIGGERS;
  arg.mask = 0x40;

  ulong i, divisor,loop_max;
  ulong div1, div2;
  uchar mode = 0;

  divisor = 8000000L / clock_rate;

  if (divisor < 65535L)
  {
    arg.val = 0x00; // 16 bit pacer clock
  }
  else
  {
    arg.val = 0xFF; // 32 bit pacer clock
  }
  select_register(file_desc,1);

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);

  select_timer_counter(file_desc,0);
  clock_mode(file_desc, 0, 2);

  if (arg.val)
  { // 32 bit mode
      if ((divisor/2) > 65535L )
        loop_max = 65535L;
      else
        loop_max = divisor / 2;
   for (i = 2 ;i <= loop_max; i++)
    {
      div1 = i;
      div2 = divisor / div1;
      if (div1*div2 == divisor)
        if (div2 <= 65535L)
       break;
    }
   if (div1*div2 != divisor)
    {
      if ((divisor/2) > 65535L )
        loop_max = 65535L;
      else
        loop_max = divisor / 2;
      for (i = 2 ;i <= loop_max; i++)
      {
        div1 = i;
        div2 = divisor / div1;
        if (div2 <= 65535L)
        break;
      }
    }
    clock_divisor(file_desc, 0, (ushort) div1); // set divisor clock 0, should be the smallest divisor.
    clock_mode(file_desc, 1, 2);
    clock_divisor(file_desc, 1, (ushort) div2); // set divisor clock 1
    return(8000000.0 / (float)(div1 * div2));
  }
  else
  { //16 bit mode
    clock_divisor(file_desc, 0, divisor);
    return(8000000.0 / (int) divisor);
  }
}

float
set_burst_clock(int file_desc, float burst_rate)
{
  select_timer_counter(file_desc, 0);
  clock_mode(file_desc, 2, 2);
  clock_divisor(file_desc, 2, 8000000L/burst_rate);
  return (8000000.0 / (int)(8000000L/burst_rate));
}

float
set_user_clock(int file_desc, uchar timer, float input_rate, float output_rate)
{
  select_timer_counter(file_desc, 1);
  clock_mode(file_desc, timer, 2);
  clock_divisor(file_desc, timer, (input_rate/output_rate));
  return (input_rate / (int) (input_rate/output_rate));
}

ushort
read_timer_counter(int file_desc, uchar timer, uchar clock)
{
  struct dm5408_io8 arg;
  uchar msb, lsb;

  select_timer_counter(file_desc, timer);

  arg.reg = REG_COUNTER_CONTROL;
  arg.val = clock << 6;
  ioctl(file_desc, DM5408_IOC_OUTB, &arg);

  arg.reg = REG_COUNTER0 + clock;
  ioctl(file_desc, DM5408_IOC_INB, &arg);
  lsb = arg.val;
  ioctl(file_desc, DM5408_IOC_INB, &arg);
  msb = arg.val;

  return ((256 * msb) + lsb);
}

void
done_timer(int file_desc)
{
  select_timer_counter(file_desc, 1);

  clock_mode(file_desc, 0, 2);
  clock_mode(file_desc, 1, 2);
  clock_divisor(file_desc, 0, 2);
  clock_divisor(file_desc, 1, 2);
}

/* dio5812.h **********************************************************/

void
select_register_5812(int file_desc, uchar selected_chip, uchar select)
{
  struct dm5408_io8 arg;

  arg.reg = REG_DIGITAL_CONTROL + (selected_chip * 4);
  arg.val = select;
  arg.mask = 0x03;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
clear_chip_5812(int file_desc, uchar selected_chip)
{
  struct dm5408_io8 arg;

  select_register_5812(file_desc, selected_chip, 0);

  arg.reg = REG_PORT0_CLEAR + (selected_chip * 4);
  arg.val = 0x00;

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
clear_irq_5812(int file_desc, uchar selected_chip)
{
  struct dm5408_io8 arg;

  select_register_5812(file_desc, selected_chip, 0);

  arg.reg = REG_CLEAR + (selected_chip * 4);

  ioctl(file_desc, DM5408_IOC_INB, &arg);
}

void
enable_irq_5812(int file_desc, uchar selected_chip, uchar enable)
{
  struct dm5408_io8 arg;

  select_register_5812(file_desc, selected_chip, 0);

  arg.reg = REG_DIGITAL_CONTROL + (selected_chip * 4);
  arg.val = enable << 4;
  arg.mask = 0x10;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_port1_direction_5812(int file_desc, uchar selected_chip, uchar direction)
{
  struct dm5408_io8 arg;

  arg.reg = REG_DIGITAL_CONTROL + (selected_chip * 4);
  arg.val = direction << 2;
  arg.mask = 0x04;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
set_port0_direction_5812(int file_desc, uchar selected_chip, uchar direction)
{
  struct dm5408_io8 arg;

  select_register_5812(file_desc, selected_chip, 1);

  arg.reg = REG_PORT0_DIRECTION + (selected_chip * 4);
  arg.val = direction;

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
load_mask_5812(int file_desc, uchar selected_chip, uchar mask)
{
  struct dm5408_io8 arg;

  select_register_5812(file_desc, selected_chip, 2);

  arg.reg = REG_PORT0_MASK + (selected_chip * 4);
  arg.val = mask;

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

void
load_compare_5812(int file_desc, uchar selected_chip, uchar compare)
{
  struct dm5408_io8 arg;

  select_register_5812(file_desc, selected_chip, 2);

  arg.reg = REG_PORT0_COMPARE + (selected_chip * 4);
  arg.val = compare;

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

uchar
read_dio_5812(int file_desc, uchar selected_chip, uchar port)
{
  struct dm5408_io8 arg;

  arg.reg = REG_DIGITAL_PORT0 + (selected_chip * 4) + port;

  ioctl(file_desc, DM5408_IOC_INB, &arg);

  return arg.val;
}

uchar
read_compare_register_5812(int file_desc, uchar selected_chip)
{
  struct dm5408_io8 arg;

  arg.reg = REG_PORT0_COMPARE + (selected_chip * 4);

  ioctl(file_desc, DM5408_IOC_INB, &arg);

  return arg.val;
}

void
select_clock_5812(int file_desc, uchar selected_chip, uchar clock)
{
  struct dm5408_io8 arg;

  arg.reg = REG_DIGITAL_CONTROL + (selected_chip * 4);
  arg.val = clock << 5;
  arg.mask = 0x20;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
select_irq_mode_5812(int file_desc, uchar selected_chip, uchar irq_mode)
{
  struct dm5408_io8 arg;

  arg.reg = REG_DIGITAL_CONTROL + (selected_chip * 4);
  arg.val = irq_mode << 3;
  arg.mask = 0x08;

  ioctl(file_desc, DM5408_IOC_MOUTB, &arg);
}

void
write_dio_5812(int file_desc, uchar selected_chip, uchar port, uchar data)
{
  struct dm5408_io8 arg;

  arg.reg = REG_DIGITAL_PORT0 + (selected_chip * 4) + port;
  arg.val = data;

  ioctl(file_desc, DM5408_IOC_OUTB, &arg);
}

uchar
is_chip_irq_5812(int file_desc, uchar selected_chip)
{
  struct dm5408_io8 arg;

  arg.reg = REG_STATUS + (selected_chip * 4);

  ioctl(file_desc, DM5408_IOC_INB, &arg);

  return (arg.val >> 7) && 1;
}
