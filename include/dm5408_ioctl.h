#ifndef __dm5408_ioctl_h__
#define __dm5408_ioctl_h__

#include <linux/ioctl.h>
#include <linux/types.h>

#define IOBASE		0x0300	// Default Base Address
#define IOSPACE		16		// IO Space Range
#define IRQS		2		// Number of IRQ's
#define DMAS		2		// Number of DMA's
#define FREQ		8000000	// Working Freqency
#define MAX_DEVS	4		// Maximum Number of Devices
#define DEV_NAME	"/dev/dm5408"	// Device Path

#ifdef __cplusplus
extern "C" {
#endif

// DM5408 Device Registers with offset
enum dm5408_registers_t
{
	REG_STATUS				= 0,	// R	- Status Register
	REG_CONTROL				= 0,	// W	- Control Register
	REG_START_CONVERSION	= 1,	// R	- Conversion
	REG_GAIN				= 1,	// W	- Channel/Gain Register
	REG_AD_TABLE			= 1,	// W	- AD Table
	REG_DIGITAL_TABLE		= 1,	// W	- Digital Table
	REG_ADC_LSB				= 2,	// R	- ADC Data LSB
	REG_TRIGGERS			= 2,	// W	- Triggers
	REG_CLOCKS				= 2,	// W	- Clocks
	REG_IRQS				= 2,	// W	- IRQ Sources
	REG_ADC_MSB				= 3,	// R	- ADC Data MSB
	REG_CLEAR				= 3,	// W	- Clear Settings
	REG_DIGITAL_PORT0		= 4,	// R/W	- Digital IO Port0
	REG_DIGITAL_PORT1		= 5,	// R/W	- Digital IO Port1
	REG_PORT0_CLEAR			= 6,	// R/W	- Digital IO Port0 Clear
	REG_PORT0_DIRECTION		= 6,	// R/W	- Digital IO Port0 Direction
	REG_PORT0_MASK			= 6,	// R/W	- Digital IO Port0 Mask
	REG_PORT0_COMPARE		= 6,	// R/W	- Digital IO Port0 Compare
	REG_DIGITAL_STATUS		= 7,	// R	- Digital Status Register
	REG_DIGITAL_CONTROL		= 7,	// W	- Digital Control Register
	REG_COUNTER0			= 8,	// R/W	- 8254 Counter0
	REG_COUNTER1			= 9,	// R/W	- 8254 Counter1
	REG_COUNTER2			= 10,	// R/W	- 8254 Counter2
	REG_COUNTER_CONTROL		= 11,	// W	- Counters Control
	REG_CLEAR_SOFT_IRQ		= 12,	// R	- Clear Software Selectable IRQ
	REG_DAC1_LSB			= 12,	// W	- DAC1 Data LSB
	REG_CLEAR_DMA_FLAG		= 13,	// R	- Clear DMA Done Flag
	REG_DAC1_MSB			= 13,	// W	- DAC1 Data MSB
	REG_LOAD_SAMPLE_COUNTER	= 14,	// R	- Software Trigger to Load Sample Counter
	REG_DAC2_LSB			= 14,	// W	- DAC2 Data LSB
	REG_UPDATE_DACS			= 15,	// R	- Update DAC's Outputs
	REG_DAC2_MSB			= 15,	// W	- DAC2 Data MSB
};

// DM5408 Clear Flags for BASE+0. ToMove to a header
enum dm5408_clr_flags_t
{
	CLEAR_FIFO			= 0x00,	// Clear FIFO
	CLEAR_GAIN_TABLE	= 0x01,	// Clear Channel-Gain Table
	RESET_GAIN_TABLE	= 0x10,	// Reset Channel-Gain Table
	CLEAR_BOARD			= 0x11,	// Clear Board
};

#ifdef __cplusplus
} // extern "C"
#endif
#endif //__dm5408_ioctl_h__
