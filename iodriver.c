#include <linux/kernel.h>
#include <linux/module.h>

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