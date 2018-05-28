#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "devioctl.h"
#include <linux/delay.h>
#include <linux/fs.h>


static int Device_open = 0;
static int Delay = 0;

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
	msleep(Delay);
	while(len)
	{
		get_user(*(msgPtr++),buffer++);
		len--;
		bytesWritten++;
	}

	return bytesWritten; 
}

static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_ALERT "device open(%p)\n",file);
	Device_open++;
	msgPtr = msg;

	try_get_module(THIS_MODULE);

	return 0;	
}

static int device_release(struct inode *inode,struct file *file)
{
	printk(KERN_ALERT "device_release(%p,%p)",inode,file);
	Device_open --;
	module_put(THIS_MODULE);
	return 0;
}

static long device_ioctl(struct inode *inode,struct file *file,unsigned int ioctl_num,unsigned long ioctl_param)
{
	Delay = (int)(ioctl_param);
	return 0;
}

struct file_operations fops = 
{
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = device_ioctl,
	.open = device_open,
	.release = device_release,
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
