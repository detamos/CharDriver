#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "devlib.h"
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/syscalls.h>

static ssize_t device_read(struct file *filp,char *buffer,size_t len,loff_t *offset)
{
	int bytesRead = 0;
	static int fixed = 0;

	if(fixed == 1)
	{
		fixed = 0;
		return 0;
	}

	int length = len;
	while(length && total != MAX)
	{
		char temp;
		if(put_user(temp,buffer++))
		{
			return -EFAULT;
		}
		
		if(rear == MAX-1)
			rear = -1;
		msg[++rear] = temp;
		total++;
		
		length --;
		bytesRead ++;
	}

	fixed = 1;

	return bytesRead; 
}
static ssize_t device_write(struct file *filp,const char *buffer,size_t len,loff_t *offset)
{
	int bytesWritten = 0;
	int length = strlen(buffer);

	char *ptrBuf = buffer;
	while(total && length)
	{
		char temp = msg[front++];
		if(front == MAX)
			front = 0;
		total--;

		get_user(temp,ptrBuf++);
		
		bytesWritten++;
		length--;
	}

	return bytesWritten; 
}

static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "device open(%p)\n",file);
	Device_open++;
//	try_module_get(THIS_MODULE);

	return 0;	
}

static int device_release(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "device_release(%p,%p)",inode,file);
	Device_open --;
//	module_put(THIS_MODULE);
	return 0;
}

static long device_ioctl(struct file *file,unsigned int ioctl_num,unsigned long ioctl_param)
{
	if(ioctl_num == IOCTL_SET_DELAY)
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
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if(major < 0)
	{
		printk(KERN_INFO "Can't Register Device %s \n",DEVICE_NAME);
		return -EFAULT;
	}
	printk(KERN_INFO "Loading the module : iodriver with device id:%d\n",major);

	return 0;
}

static void __exit remove_module(void)
{
	unregister_chrdev(major,DEVICE_NAME);
	printk(KERN_INFO "Removing the module : iodriver\n");
}

module_init(load_module);
module_exit(remove_module);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Detamos");
MODULE_DESCRIPTION("Non-blocking read/write driver");
MODULE_SUPPORTED_DEVICE("iitpipe");
