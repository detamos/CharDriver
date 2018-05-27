#include <linux/kernel.h>
#include <linux/module.h>
#include "devioctl.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE(DEVICE_FILE);

static int __init load_module(void)
{
	printk(KERN_ALERT "Loading the module : iodriver\n");
	return 0;
}

static void __exit remove_module(void)
{
	printk(KERN_ALERT "Removing the module : iodriver\n");
}

module_init(load_module);
module_exit(remove_module);
