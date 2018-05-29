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

static int Device_open = 0;
static int Delay = 0;
char msg1[MAX];
char msg2[MAX];
int front2 = 0;
int rear2 = -1;
int total2 = 0;
int front1 = 0;
int rear1 = -1;
int total1 = 0;
char tempData;

ssize_t device_read(struct file *filp,char *buffer,size_t len,loff_t *offset)
{
	int bytesRead = 0;
	static int fixed = 0;

	if(fixed == 1)
	{
		fixed = 0;
		return 0;
	}
	int total;
	
	if(iminor(filp->f_path.dentry->d_inode) == 0)
	{
		total = total1;
	}
	else
	{
		total = total2;
	}
	size_t length = len;
	while(length && total)
	{
		if(put_user(tempData,buffer++))
		{
			return -EFAULT;
		}
		if(iminor(filp->f_path.dentry->d_inode) == 0)
		{
			tempData = msg1[front1++];
			if(front1 == MAX)
				front1 = 0;
			total1--;
			total--;	
		}
		else
		{
			tempData = msg2[front2++];
			if(front2 == MAX)
				front2 = 0;
			total2--;
			total--;
		}
		
		length --;
		bytesRead ++;
	}

	fixed = 1;
	printk(KERN_INFO "Minor Number %d read %d bytes\n",iminor(filp->f_path.dentry->d_inode),bytesRead);
	return bytesRead; 
}
ssize_t device_write(struct file *filp,const char *buffer,size_t len,loff_t *offset)
{
	int bytesWritten = 0;
	int length = strlen(buffer);

	int total;
	if(iminor(filp->f_path.dentry->d_inode) == 0)
	{
		total = total1;
	}
	else
	{
		total = total2;
	}
	while(total != MAX && length)
	{
		if(iminor(filp->f_path.dentry->d_inode) == 0)
		{
			if(rear1 == MAX-1)
				rear1 = -1;
			msg1[++rear1] = tempData;
			total1++;
			total++;
		}	
		else
		{
			if(rear2 == MAX-1)
				rear2 = -1;
			msg2[++rear2] = tempData;
			total2++;
			total++;
		}
		if(get_user(tempData,buffer++))
		{
			return -EFAULT;
		}

		bytesWritten++;
		length--;
	}
	printk(KERN_INFO "Minor Number %d wrote %d bytes\n",iminor(filp->f_path.dentry->d_inode),bytesWritten);
	if(bytesWritten == 0 && total == MAX)
		return -1;
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
	else if(ioctl_num == IOCTL_GET_DELAY)
		return Delay;
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
