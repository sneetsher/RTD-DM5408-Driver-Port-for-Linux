#include <linux/errno.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/sched.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/version.h>


static ushort iobase = 0x0300;


static int __init initialization_function(void)
{
	printk(KERN_INFO "DM5408 init_module() called with iobase=0x%04x.\n", iobase);
	return 0;
}


static void __exit cleanup_function(void)
{
	printk(KERN_INFO "DM5408 cleanup_module() called.\n");
}


module_param(iobase, ushort, S_IRUGO);

MODULE_PARM_DESC(iobase, "I/O port base address");

MODULE_AUTHOR("Abdellah Chelli <abdellahchelli@gmail.com>");
MODULE_DESCRIPTION("RTD DM5408 Acquisition Card Driver. (No IRQ/DMA Support)");
MODULE_SUPPORTED_DEVICE("dm5408");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("dm408_mod");


module_init(initialization_function);
module_exit(cleanup_function);
