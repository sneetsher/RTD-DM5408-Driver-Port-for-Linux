#define DEBUG

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

#include "../include/dm5408_ioctl.h"


static const char dev_name[] = "dm5408";
static ushort io_base[MAX_DEVS] = {IOBASE};	// Module Parameter
static uchar dev_major = 0;

// DM5408 Device Structure with spinlock
typedef struct dm5408_device
{
	ushort		io_base;		// Base address
	uchar		initialized;	// Initialization flag
	uchar		reg_control,	// Temp variables
				reg_trigger0,
				reg_trigger1,
				reg_irq;
	atomic_t	use_counter;	// Inuse atomic counter
	spinlock_t	spin_lock;		// Spinlock
} dm5408_device_t ;

static dm5408_device_t devices[MAX_DEVS];


// DM5408 File Operation Functions
static int dm5408_open(struct inode *inode_ptr, struct file *file_ptr)
{
	int call_result = 0; // ToDo: Check if still needed
	dm5408_device_t *dev_ptr;
	int dev_minor;

	#ifdef DEBUG
		printk(KERN_INFO "DM5408: dm5408_open() called.\n");
	#endif

	dev_minor = MINOR((inode_ptr)->i_rdev);
	dev_ptr = ( dev_minor >= MAX_DEVS)? 0 : &devices[dev_minor];
	if (!dev_ptr) return -ENXIO;
	if (!dev_ptr->io_base) return -ENXIO;

// ToDo: func extraction
	if (dev_ptr->initialized || !dev_ptr->io_base) return call_result;
	if (request_region(dev_ptr->io_base, IOSPACE, dev_name) == NULL)
	{
		printk(KERN_ERR "DM5408: Requesting IO region faild %04x-%04x.\n",
			dev_ptr->io_base, dev_ptr->io_base + IOSPACE - 1 );
		return -EBUSY;
	};
	printk(KERN_ERR "DM5408: Request IO region %04x-%04x successful.\n",
			dev_ptr->io_base, dev_ptr->io_base + IOSPACE - 1 );

	dev_ptr->reg_control = 0;
	dev_ptr->reg_trigger0 = 0;
	dev_ptr->reg_trigger1 = 0;
	dev_ptr->reg_irq = 0;
	// ToDo: outb regs

	dev_ptr->initialized = 1;
// ToDo: func extraction end

// ToDo: to convert
//DM6420HRClear()
// ToDo: to convert end

	if (file_ptr->private_data) return -EBUSY;

    file_ptr->private_data = dev_ptr;
    atomic_inc(&dev_ptr->use_counter);

	// ToDo: Add code here.

	#ifdef DEBUG
		printk(KERN_INFO "DM5408: dm5408_open() completed.\n");
	#endif

	return call_result;
};

static int dm5408_release(struct inode *inode_ptr, struct file *file_ptr)
{
	dm5408_device_t *dev_ptr;
	dev_ptr = (dm5408_device_t *) file_ptr->private_data;

	if (dev_ptr)
	{
		if (atomic_dec_and_test(&dev_ptr->use_counter) && dev_ptr->initialized)
		{
			if (dev_ptr->io_base)
				release_region(dev_ptr->io_base, IOSPACE);
			atomic_set(&dev_ptr->use_counter, 0);
			dev_ptr->initialized = 0;
		};
	};
	file_ptr->private_data = 0;
	return 0;
};

static int dm5408_ioctl(struct inode *inode_ptr, struct file *file_ptr, uint cmd, ulong arg)
{
	return 0;
};


// DM5408 File Operation Structure
static struct file_operations dev_fops =
{
    .owner =    THIS_MODULE,
    .open =     dm5408_open,
    .ioctl =    dm5408_ioctl,
    .release =  dm5408_release,
};


// Module Init Function
static int __init initialization_function(void)
{

	int dev_count;
	ushort dev_addr;
	int dev_addr_valid = 0;
	int call_result;

	#ifdef DEBUG
		int i;
		for(i=0; i<MAX_DEVS; i++)
		{
			printk(KERN_INFO "DM5408: init_module() called with io_base=0x%04x.\n", io_base[i]);
		};
	#endif

	// Addresses Validation
	for( dev_count = 0; dev_count < MAX_DEVS; dev_count++)
	{
		dev_addr = io_base[dev_count];
		if (!dev_addr) continue;
		if ((dev_addr < 0x0300) || (dev_addr > 0x03F0) || (dev_addr & 0x000F)) {
			printk(KERN_ERR "DM5408: invalid base address 0x%04x.\n", dev_addr);
        return -EINVAL;
		};
		dev_addr_valid++;
	};

	// At least one valid address, No need to this as Default address always present
	if (dev_addr_valid == 0)
	{
		printk(KERN_ERR "DM5408: No valid addresses.\n");
		return -ENODEV;
	};

	// Char Device Registration
	call_result = register_chrdev(dev_major, dev_name, &dev_fops);
	if (call_result < 0)
	{
		printk(KERN_ERR "DM5408: Character device registration failed, ErrNo = %d.\n", -call_result);
		return call_result;
	};
	dev_major = call_result;

	#ifdef DEBUG
		printk(KERN_INFO "DM5408: Char device registered with major = %d\n", dev_major);
	#endif

	for( dev_count = 0; dev_count < MAX_DEVS; dev_count++)
	{
		dm5408_device_t *dev_ptr = &devices[dev_count];
		atomic_set(&dev_ptr->use_counter, 0);
		dev_ptr->spin_lock = SPIN_LOCK_UNLOCKED;
		dev_ptr->io_base = io_base[dev_count];
		dev_ptr->initialized = 0;
	};

	// ToDo: Continue code here. workqueue ???

	#ifdef DEBUG
		printk(KERN_INFO "DM5408: init_module() completed.\n");
	#endif

	return 0;
}


// Module Cleanup Function
static void __exit cleanup_function(void)
{
	#ifdef DEBUG
		printk(KERN_INFO "DM5408: cleanup_module() called.\n");
	#endif

	unregister_chrdev( dev_major, dev_name);

	// ToDo: Continue code here

	#ifdef DEBUG
		printk(KERN_INFO "DM5408: cleanup_module() completed.\n");
	#endif
}


// Module Parameters
module_param_array(io_base, ushort, NULL, S_IRUGO);

MODULE_PARM_DESC(io_base, " I/O port base addresses");


// Module Info
MODULE_AUTHOR("Abdellah Chelli <abdellahchelli@gmail.com>");
MODULE_DESCRIPTION("RTD DM5408 Acquisition Card Driver. (No IRQ/DMA Support)");
MODULE_SUPPORTED_DEVICE("dm5408");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("dm408_mod");


// Module Init/Exit Functions
module_init(initialization_function);
module_exit(cleanup_function);
