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

#include "../include/dm5408_mod.h"
#include "../include/dm5408_ioctl.h"

static ushort io_base = IOBASE;	// Module Parameter

// Module Init Function
static int __init initialization_function(void)
{
	printk(KERN_INFO "DM5408 init_module() called with iobase=0x%04x.\n", io_base);
	return 0;
}

// Module Cleanup Function
static void __exit cleanup_function(void)
{
	printk(KERN_INFO "DM5408 cleanup_module() called.\n");
}

// Module Parameters
module_param(io_base, ushort, S_IRUGO);

MODULE_PARM_DESC(io_base, "I/O port base address");

// Module Info
MODULE_AUTHOR("Abdellah Chelli <abdellahchelli@gmail.com>");
MODULE_DESCRIPTION("RTD DM5408 Acquisition Card Driver. (No IRQ/DMA Support)");
MODULE_SUPPORTED_DEVICE("dm5408");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("dm408_mod");

// Module Init/Exit Functions
module_init(initialization_function);
module_exit(cleanup_function);
