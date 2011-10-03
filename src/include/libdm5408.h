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

#ifndef __dm5408_lib_h__
#define __dm5408_lib_h__

#include <sys/ioctl.h>
#include <sys/types.h>

#include "dm5408_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ad_table_row
{
  uchar channel,
        gain,
        skip;
};
typedef struct ad_table_row ad_table_row_t;

// BA+0 R
enum status_reg_mask
{
  ADC_FIFO_EMPTY  = 0x01,
  ADC_FIFO_FULL   = 0x02,
  ADC_HALTED      = 0x04,
  ADC_CONVERTING  = 0x08,
  PACER_CLOCK_ON  = 0x10,
  ADC_DMA_DONE    = 0x20,
  IRQ             = 0x40,
  DIGITAL_IRQ     = 0x80,
};

// BASE+0.
enum select_reg_val
{
  TRIGGER1_REG      = 0x00,
  CLEAR_FIFO        = 0x00, // Clear FIFO
  TRIGGER2_REG      = 0x01,
  CLEAR_GAIN_TABLE  = 0x01, // Clear Channel-Gain Table
  IRQ_REG           = 0x02,
  RESET_GAIN_TABLE  = 0x02, // Reset Channel-Gain Table
  RESERVED          = 0x03,
  CLEAR_BOARD       = 0x03, // Clear Board
};

void init_board(int);
uchar read_status_register(int);
uchar is_adc_fifo_empty(uchar);
uchar is_adc_fifo_full(uchar);
uchar is_adc_halted(uchar);
uchar is_adc_converting(uchar);
uchar is_pacer_clock_on(uchar);
uchar is_adc_dma_done(uchar);
uchar is_irq(uchar);
uchar is_digital_irq(uchar);
void write_control_register(int, uchar);
void select_register(int, enum select_reg_val);
void select_timer_counter(int, uchar);
void enable_channel_gain_data_store(int, uchar);
void enable_tables(int, uchar, uchar);
void start_conversion(int);
void set_channel_gain(int, uchar, uchar);
void load_ad_table(int, uchar, ad_table_row_t*);
void load_digital_table(int, uchar, uchar*);
short read_ad_data(int);
ushort read_channel_gain_data_store(int);
short read_ad_data_with_marker(int);
void load_trigger0_register(int, uchar);
void load_trigger1_register(int, uchar);
void load_irq_register(int, uchar);
void set_conversion_select(int, uchar);
void set_start_trigger(int, uchar);
void set_stop_trigger(int, uchar);
void set_pacer_clock_source(int, uchar);
void set_burst_trigger(int, uchar);
void set_trigger_polarity(int, uchar);
void set_trigger_repeat(int, uchar);
void set_sample_counter_stop(int, uchar);
void set_irq_source(int, uchar);
void enable_irq(int, uchar);
void enable_irq_sharing(int, uchar);
void digital_irq_mask(int, uchar);
void clear_ad_fifo(int);
void clear_channel_gain_table(int);
void reset_channel_gain_table(int);
void clear_board(int);
void clear_irq(int);
void clear_ad_dma_done(int);
void load_dac(int, uchar, ushort);
void update_dacs(int);
void clock_mode(int, uchar, uchar);
void clock_divisor(int, uchar, ushort);
float set_pacer_clock(int ,float);
float set_burst_clock(int, float);
float set_user_clock(int, uchar, float, float);
ushort read_timer_counter(int, uchar, uchar);
void done_timer(int);

//dio5812.h
void select_register_5812(int, uchar, uchar);
void clear_chip_5812(int, uchar);
void clear_irq_5812(int , uchar);
void enable_irq_5812(int, uchar, uchar);
void set_port1_direction_5812(int, uchar, uchar);
void set_port0_direction_5812(int, uchar, uchar);
void load_mask_5812(int, uchar, uchar);
void load_compare_5812(int, uchar, uchar);
uchar read_dio_5812(int, uchar, uchar);
uchar read_compare_register_5812(int, uchar);
void select_clock_5812(int, uchar, uchar);
void select_irq_mode_5812(int, uchar, uchar);
void write_dio_5812(int, uchar, uchar, uchar);
uchar is_chip_irq_5812(int, uchar);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //__dm5408_lib_h__
