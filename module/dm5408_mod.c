#include <linux/module.h>	
#include <linux/kernel.h>	


MODULE_AUTHOR("Abdellah Chelli <abdellahchelli@gmail.com>");
MODULE_LICENSE("LGPL");


int init_module(void)
{
	printk(KERN_INFO "dm5408_init_module() called\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "dm5408_cleanup_module() called\n");
}
