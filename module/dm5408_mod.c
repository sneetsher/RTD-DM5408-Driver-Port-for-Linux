#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/ioctl.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/stat.h>
#include <linux/time.h>
#include <linux/types.h>
#include <linux/version.h>

#include <asm/atomic.h>
#include <asm/byteorder.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define IOBASE 0x0300
#define IOSPACE 16
#define FREQ 8000000

static ushort io_base = IOBASE;

typedef struct dm5408_device
{
	ushort		io_base;
	ushort		control_register,
				datain_register;
	atomic_t	inuse_counter;
	spinlock_t	spin_lock;
} dm5408_device_t ;

enum dm5408_registers
{
	REG_STATUS				= 0,	// R	- Status Register
	REG_CONTROL				= 0,	// W	- Control Register
	REG_START_CONVERSION	= 1,	// R	- Conversion
	REG_GAIN				= 1,	// W	- Channel/Gain Register
	REG_ADC_TABLE			= 1,	// W	- AD Table
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


static int __init initialization_function(void)
{
	printk(KERN_INFO "DM5408 init_module() called with iobase=0x%04x.\n", io_base);
	return 0;
}


static void __exit cleanup_function(void)
{
	printk(KERN_INFO "DM5408 cleanup_module() called.\n");
}


module_param(io_base, ushort, S_IRUGO);

MODULE_PARM_DESC(io_base, "I/O port base address");

MODULE_AUTHOR("Abdellah Chelli <abdellahchelli@gmail.com>");
MODULE_DESCRIPTION("RTD DM5408 Acquisition Card Driver. (No IRQ/DMA Support)");
MODULE_SUPPORTED_DEVICE("dm5408");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("dm408_mod");


module_init(initialization_function);
module_exit(cleanup_function);
