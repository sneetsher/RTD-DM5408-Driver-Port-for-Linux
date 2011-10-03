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

#ifndef __dm5408_ioctl_h__
#define __dm5408_ioctl_h__

#include <linux/ioctl.h>
#include <linux/types.h>

// ToDo: check is it valid, char may be unsigned by default
typedef unsigned char uchar;
typedef signed char schar;


#define IOBASE      0x0300  // Default Base Address
#define IOBASE_MIN  0x0300  // Minimum Base Address
#define IOBASE_MAX  0x03F0  // Maximum Base Address
#define IOSPACE     16      // IO Space Range
#define IRQS        2       // Number of IRQ's
#define DMAS        2       // Number of DMA's
#define FREQ        8000000 // Working Freqency
#define MAX_DEVS    4       // Maximum Number of Devices
#define DEV_NAME    "/dev/dm5408"   // Device Path


#ifdef __cplusplus
extern "C" {
#endif


// DM5408 Device Registers with offset
enum dm5408_registers
{
  REG_STATUS              = 0,  // R    - Status Register
  REG_CONTROL             = 0,  // W    - Control Register
  REG_START_CONVERSION    = 1,  // R    - Conversion
  REG_GAIN                = 1,  // W    - Channel/Gain Register
  REG_AD_TABLE            = 1,  // W    - AD Table
  REG_DIGITAL_TABLE       = 1,  // W    - Digital Table
  REG_ADC_LSB             = 2,  // R    - ADC Data LSB
  REG_TRIGGERS            = 2,  // W    - Triggers
  REG_CLOCKS              = 2,  // W    - Clocks
  REG_IRQS                = 2,  // W    - IRQ Sources
  REG_ADC_MSB             = 3,  // R    - ADC Data MSB
  REG_CLEAR               = 3,  // W    - Clear Settings
  REG_DIGITAL_PORT0       = 4,  // R/W  - Digital IO Port0
  REG_DIGITAL_PORT1       = 5,  // R/W  - Digital IO Port1
  REG_PORT0_CLEAR         = 6,  // R/W  - Digital IO Port0 Clear
  REG_PORT0_DIRECTION     = 6,  // R/W  - Digital IO Port0 Direction
  REG_PORT0_MASK          = 6,  // R/W  - Digital IO Port0 Mask
  REG_PORT0_COMPARE       = 6,  // R/W  - Digital IO Port0 Compare
  REG_DIGITAL_STATUS      = 7,  // R    - Digital Status Register
  REG_DIGITAL_CONTROL     = 7,  // W    - Digital Control Register
  REG_COUNTER0            = 8,  // R/W  - 8254 Counter0
  REG_COUNTER1            = 9,  // R/W  - 8254 Counter1
  REG_COUNTER2            = 10, // R/W  - 8254 Counter2
  REG_COUNTER_CONTROL     = 11, // W    - Counters Control
  REG_CLEAR_SOFT_IRQ      = 12, // R    - Clear Software Selectable IRQ
  REG_DAC1_LSB            = 12, // W    - DAC1 Data LSB
  REG_CLEAR_DMA_FLAG      = 13, // R    - Clear DMA Done Flag
  REG_DAC1_MSB            = 13, // W    - DAC1 Data MSB
  REG_LOAD_SAMPLE_COUNTER = 14, // R    - Software Trigger to Load Sample Counter
  REG_DAC2_LSB            = 14, // W    - DAC2 Data LSB
  REG_UPDATE_DACS         = 15, // R    - Update DAC's Outputs
  REG_DAC2_MSB            = 15, // W    - DAC2 Data MSB
};

// ioctl structures
struct dm5408_io8
{
    enum dm5408_registers   reg;
    uchar                   val;
    uchar                   mask;
};

typedef struct dm5408_io8 dm5408_io8_t;

struct dm5408_io16
{
    enum dm5408_registers   reg;
    ushort                  val;
    ushort                  mask;
};

typedef struct dm5408_io16 dm5408_io16_t;

// ioctl codes
#define __DM5408_IOC_MAGIC 0xD5

#define DM5408_IOC_INB      _IOWR (__DM5408_IOC_MAGIC, 0x00, dm5408_io8_t)
#define DM5408_IOC_OUTB     _IOW  (__DM5408_IOC_MAGIC, 0x01, dm5408_io8_t)
#define DM5408_IOC_MOUTB    _IOW  (__DM5408_IOC_MAGIC, 0x02, dm5408_io8_t)
#define DM5408_IOC_INW      _IOWR (__DM5408_IOC_MAGIC, 0x03, dm5408_io16_t)
#define DM5408_IOC_OUTW     _IOW  (__DM5408_IOC_MAGIC, 0x04, dm5408_io16_t)
#define DM5408_IOC_MOUTW    _IOW  (__DM5408_IOC_MAGIC, 0x05, dm5408_io16_t)


#ifdef __cplusplus
} // extern "C"
#endif
#endif //__dm5408_ioctl_h__
