#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include "devioctl.h"



struct file_operations fops = 
{
	.read = device_read,
	.write = device_write
};

static int __init load_module(void)
{
	printk(KERN_ALERT "Loading the module : iodriver\n");
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if(major < 0)
	{
		printk(KERN_ALERT "Can't Register Device %s \n",DEVICE_NAME);
		return -EFAULT;
	}

	create_devFiles(major);

	return 0;
}

static void __exit remove_module(void)
{
	remove_devFiles(major);
	
	printk(KERN_ALERT "Removing the module : iodriver\n");
}

module_init(load_module);
module_exit(remove_module);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Detamos");
MODULE_DESCRIPTION("Non-blocking read/write driver");
MODULE_SUPPORTED_DEVICE("iitpipe");
