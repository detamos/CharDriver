#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "devioctl.h"

char *msgPtr = NULL;
char *msg = "Hello World!";

static ssize_t device_read(struct file *filp,char *buffer,size_t len,loff_t *offset)
{
	int bytesRead = 0;
	static int fixed = 0;
	int length = strlen(msg);

	if(fixed == 1)
	{
		fixed = 0;
		return 0;
	}

	while(length && buffer != NULL)
	{
		if(put_user(*(msgPtr++),buffer++))
		{
			return -EFAULT;
		}
		length --;
		bytesRead ++;
	}

	fixed = 1;

	return bytesRead; 
}
static ssize_t device_write(struct file *filp,const char *buffer,size_t len,loff_t *offset)
{
	int bytesWritten = 0;

	while(len)
	{
		get_user(*(msgPtr++),buffer++);
		len--;
		bytesWritten++;
	}

	return bytesWritten; 
}

struct file_operations fops = 
{
	.read = device_read,
	.write = device_write
};

static int __init load_module(void)
{
	msgPtr = msg;
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if(major < 0)
	{
		printk(KERN_ALERT "Can't Register Device %s \n",DEVICE_NAME);
		return -EFAULT;
	}
	printk(KERN_ALERT "Loading the module : iodriver with device id:%d\n",major);

	return 0;
}

static void __exit remove_module(void)
{
	unregister_chrdev(major,DEVICE_NAME);

	printk(KERN_ALERT "Removing the module : iodriver\n");
}

module_init(load_module);
module_exit(remove_module);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Detamos");
MODULE_DESCRIPTION("Non-blocking read/write driver");
MODULE_SUPPORTED_DEVICE("iitpipe");
