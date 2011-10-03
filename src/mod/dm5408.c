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

#include "dm5408_ioctl.h"


static const char dev_name[] = "dm5408";
static ushort io_base[MAX_DEVS] = {IOBASE};  // Module Parameter, default address
static uchar dev_major = 0;

// DM5408 Device Structure with spinlock
struct dm5408_device
{
  ushort     io_base;      // Base address
  uchar      initialized;  // Initialization flag
  uchar      reg_control,  // Temp variables
             reg_trigger0,
             reg_trigger1,
             reg_irq;
  atomic_t   use_counter;  // Inuse atomic counter
  spinlock_t spin_lock;    // Spinlock
};

typedef struct dm5408_device dm5408_device_t;

static dm5408_device_t devices[MAX_DEVS];


// DM5408 File Operation Functions
static int
dm5408_open(struct inode *inode_ptr, struct file *file_ptr)
{
  int ret_val = 0; // ToDo: Check if still needed
  int dev_minor;
  dm5408_device_t *dev_ptr;

  #ifdef DEBUG
    printk(KERN_INFO "DM5408: dm5408_open() called.\n");
  #endif

  dev_minor = MINOR(inode_ptr->i_rdev);
  dev_ptr = (dev_minor >= MAX_DEVS)? 0 : &devices[dev_minor];

  if (!dev_ptr)
    return -ENXIO;
  if (!dev_ptr->io_base)
    return -ENXIO;
  if (dev_ptr->initialized || !dev_ptr->io_base)
    return ret_val;

  if (request_region(dev_ptr->io_base, IOSPACE, dev_name) == NULL)
  {
    printk(KERN_ERR "DM5408: Requesting IO region faild %04X-%04X.\n"
            , dev_ptr->io_base, dev_ptr->io_base + IOSPACE - 1 );
    return -EBUSY;
  };
  printk(KERN_ERR "DM5408: Request IO region %04X-%04X successful.\n"
            , dev_ptr->io_base, dev_ptr->io_base + IOSPACE - 1 );

  dev_ptr->reg_control = 0;
  dev_ptr->reg_trigger0 = 0;
  dev_ptr->reg_trigger1 = 0;
  dev_ptr->reg_irq = 0;

  dev_ptr->initialized = 1;

  if (file_ptr->private_data)
    return -EBUSY;

    file_ptr->private_data = dev_ptr;
    atomic_inc(&dev_ptr->use_counter);

  // ToDo: Add code here.

  #ifdef DEBUG
    printk(KERN_INFO "DM5408: dm5408_open() completed.\n");
  #endif

  return ret_val;
};

static int
dm5408_release(struct inode *inode_ptr, struct file *file_ptr)
{
  dm5408_device_t *dev_ptr;

  #ifdef DEBUG
    printk(KERN_INFO "DM5408: dm5408_release() called.\n");
  #endif

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

  #ifdef DEBUG
    printk(KERN_INFO "DM5408: dm5408_release() completed.\n");
  #endif

  return 0;
};

static int
dm5408_ioctl(struct inode *inode_ptr, struct file *file_ptr, uint cmd, ulong arg)
{
  int    ret_val = 0;
  ulong  lock_flags;
  uchar  val8;
  ushort  val16;
  dm5408_device_t *dev_ptr;
  dm5408_io8_t  data_io8;
  dm5408_io16_t  data_io16;


  #ifdef DEBUG
    printk(KERN_INFO "DM5408: dm5408_ioctl() called.\n");
  #endif

  dev_ptr = (dm5408_device_t *) file_ptr->private_data;
  if (!dev_ptr)
    return -EINVAL;
  switch (cmd)
  {
    case DM5408_IOC_INB:
      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() INB called.\n");
      #endif

      if (!(file_ptr->f_mode & FMODE_READ))
        return -EACCES;
      if (copy_from_user(&data_io8, (dm5408_io8_t *) arg, sizeof(data_io8)))
        return -EFAULT;
      if (data_io8.reg >= IOSPACE)
        return -EINVAL;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() INB called \
                for 0x%04X + 0x%04X.\n"
                , dev_ptr->io_base, data_io8.reg);
      #endif

      spin_lock_irqsave( &dev_ptr->spin_lock, lock_flags);
      data_io8.val = inb_p(dev_ptr->io_base + data_io8.reg);
      spin_unlock_irqrestore( &dev_ptr->spin_lock, lock_flags);

      if (copy_to_user((dm5408_io8_t *) arg, &data_io8, sizeof(data_io8)))
        return -EFAULT;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() INB completed with 0x%02X.\n"
                , data_io8.val);
      #endif
    break;

    case DM5408_IOC_OUTB:
      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() OUTB called.\n");
      #endif

      if (!(file_ptr->f_mode & FMODE_WRITE))
        return -EACCES;
      if (copy_from_user(&data_io8, (dm5408_io8_t *) arg, sizeof(data_io8)))
        return -EFAULT;
      if (data_io8.reg >= IOSPACE)
        return -EINVAL;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() OUTB called \
                for 0x%04X + 0x%04X <- 0x%02X.\n"
                , dev_ptr->io_base, data_io8.reg, data_io8.val);
      #endif

      spin_lock_irqsave( &dev_ptr->spin_lock, lock_flags);
      outb_p(dev_ptr->io_base + data_io8.reg, data_io8.val);
      spin_unlock_irqrestore( &dev_ptr->spin_lock, lock_flags);

      if (copy_to_user((dm5408_io8_t *) arg, &data_io8, sizeof(data_io8)))
        return -EFAULT;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() OUTB completed.\n");
      #endif
    break;

    case DM5408_IOC_MOUTB:
      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() MOUTB called.\n");
      #endif

      if (!(file_ptr->f_mode & FMODE_WRITE))
        return -EACCES;
      if (copy_from_user(&data_io8, (dm5408_io8_t *) arg, sizeof(data_io8)))
        return -EFAULT;
      if (data_io8.reg >= IOSPACE)
        return -EINVAL;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() MOUTB called \
                for 0x%04X + 0x%04X <- 0x%02X mask 0x%02X.\n"
                , dev_ptr->io_base, data_io8.reg, data_io8.val, data_io8.mask);
      #endif

      spin_lock_irqsave( &dev_ptr->spin_lock, lock_flags);
      val8 = inb_p(dev_ptr->io_base + data_io8.reg);
      val8 = (val8 & data_io8.mask) | (data_io8.val & ~data_io8.mask);
      outb_p(dev_ptr->io_base + data_io8.reg, data_io8.val);
      spin_unlock_irqrestore( &dev_ptr->spin_lock, lock_flags);

      if (copy_to_user((dm5408_io8_t *) arg, &data_io8, sizeof(data_io8)))
        return -EFAULT;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() MOUTB completed.\n");
      #endif
    break;

    case DM5408_IOC_INW:
      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() INW called.\n");
      #endif
      if (!(file_ptr->f_mode & FMODE_READ))
        return -EACCES;
      if (copy_from_user(&data_io16, (dm5408_io16_t *) arg, sizeof(data_io16)))
        return -EFAULT;
      if ((data_io16.reg > IOSPACE - 2) || (data_io16.reg & 1))
        return -EINVAL;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() INW called \
                for 0x%04X + 0x%04X.\n"
                , dev_ptr->io_base, data_io16.reg);
      #endif

      spin_lock_irqsave( &dev_ptr->spin_lock, lock_flags);
      data_io16.val = inb_p(dev_ptr->io_base + data_io16.reg);
      spin_unlock_irqrestore( &dev_ptr->spin_lock, lock_flags);

      if (copy_to_user((dm5408_io16_t *) arg, &data_io16, sizeof(data_io16)))
        return -EFAULT;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() INW completed with 0x%04X.\n"
                , data_io16.val);
      #endif
    break;

    case DM5408_IOC_OUTW:
      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() OUTW called.\n");
      #endif

      if (!(file_ptr->f_mode & FMODE_WRITE))
        return -EACCES;
      if (copy_from_user(&data_io16, (dm5408_io16_t *) arg, sizeof(data_io16)))
        return -EFAULT;
      if ((data_io16.reg > IOSPACE - 2) || (data_io16.reg & 1))
        return -EINVAL;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() OUTW called \
                for 0x%04X + 0x%04X <- 0x%04X.\n"
                , dev_ptr->io_base, data_io16.reg, data_io16.val);
      #endif

      spin_lock_irqsave( &dev_ptr->spin_lock, lock_flags);
      outb_p(dev_ptr->io_base + data_io16.reg, data_io16.val);
      spin_unlock_irqrestore( &dev_ptr->spin_lock, lock_flags);

      if (copy_to_user((dm5408_io16_t *) arg, &data_io16, sizeof(data_io16)))
        return -EFAULT;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() OUTW completed.\n");
      #endif
    break;

    case DM5408_IOC_MOUTW:
      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() MOUTW called.\n");
      #endif

      if (!(file_ptr->f_mode & FMODE_WRITE))
        return -EACCES;
      if (copy_from_user(&data_io16, (dm5408_io16_t *) arg, sizeof(data_io16)))
        return -EFAULT;
      if ((data_io16.reg > IOSPACE - 2) || (data_io16.reg & 1))
        return -EINVAL;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() MOUTW called \
                for 0x%04X + 0x%04X <- 0x%04X mask 0x%04X.\n"
                , dev_ptr->io_base, data_io16.reg, data_io16.val
                , data_io16.mask);
      #endif

      spin_lock_irqsave( &dev_ptr->spin_lock, lock_flags);
      val16 = inb_p(dev_ptr->io_base + data_io16.reg);
      val16 = (val16 & data_io16.mask) | (data_io16.val & ~data_io16.mask);
      outb_p(dev_ptr->io_base + data_io16.reg, data_io16.val);
      spin_unlock_irqrestore( &dev_ptr->spin_lock, lock_flags);

      if (copy_to_user((dm5408_io16_t *) arg, &data_io16, sizeof(data_io16)))
        return -EFAULT;

      #ifdef DEBUG
        printk(KERN_INFO "DM5408: dm5408_ioctl() MOUTW completed.\n");
      #endif
    break;

    default:
      ret_val = -EINVAL;
  };

  #ifdef DEBUG
    printk(KERN_INFO "DM5408: dm5408_ioctl() completed.\n");
  #endif

  return ret_val;
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
static int
__init initialization_function(void)
{
  int dev_count;
  ushort dev_addr;
  int dev_addr_valid = 0;
  int ret_val;

  #ifdef DEBUG
    int i;
    for(i=0; i<MAX_DEVS; i++)
      printk(KERN_INFO "DM5408: init_module() called with io_base=0x%04X.\n", io_base[i]);
  #endif

  // Addresses Validation
  for( dev_count = 0; dev_count < MAX_DEVS; dev_count++)
  {
    dev_addr = io_base[dev_count];
    if (!dev_addr)
        continue;
    if ((dev_addr < IOBASE_MIN) || (dev_addr > IOBASE_MAX) || (dev_addr & 0x000F)) {
      printk(KERN_ERR "DM5408: invalid base address 0x%04X.\n", dev_addr);
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
  ret_val = register_chrdev(dev_major, dev_name, &dev_fops);
  if (ret_val < 0)
  {
    printk(KERN_ERR "DM5408: Character device registration failed, ErrNo = %d.\n", -ret_val);
    return ret_val;
  };
  dev_major = ret_val;

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
static void
__exit cleanup_function(void)
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
MODULE_DESCRIPTION("RTD DM5408 Acquisition Card Driver (No IRQ/DMA Support)");
MODULE_SUPPORTED_DEVICE("dm5408");
MODULE_LICENSE("GPL");
MODULE_ALIAS("dm408");


// Module Init/Exit Functions
module_init(initialization_function);
module_exit(cleanup_function);
