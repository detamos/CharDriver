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
#include <linux/vmalloc.h>
#include <linux/random.h>
#include <linux/delay.h>

static int Device_open = 0;
static int Delay = 0;
char tempData1,tempData2[1];
int bytesRead = 0;
int bytesWritten = 0;
char *msg1;
char *msg2;
int front2 = 0;
int rear2 = -1;
int total2 = 0;
int front1 = 0;
int rear1 = -1;
int i;
int total1 = 0;

ssize_t device_read(struct file *filp,char *buffer,size_t len,loff_t *offset)
{
	bytesRead = 0;
	while(len && total1)
	{
		tempData1 = msg1[front1++];
		if(put_user(tempData1,buffer++))
		{
			return -EFAULT;
		}
		if(front1 == MAX)
			front1 = 0;
		total1--;		
		len --;
		bytesRead ++;
	}

//	printk(KERN_INFO "Minor Number %d read %d bytes\n",iminor(filp->f_path.dentry->d_inode),bytesRead);
	return bytesRead; 
}
ssize_t device_write(struct file *filp,const char *buffer,size_t len,loff_t *offset)
{
	msleep(Delay);
	bytesWritten = 0;
	while(total2 != MAX && len)
	{
		int ret = copy_from_user(tempData2,buffer++,1);
		if(rear2 == MAX-1)
			rear2 = -1;
		msg2[++rear2] = tempData2[0];
		total2++;

		bytesWritten++;
		len--;
	}
//	printk(KERN_INFO "Minor Number %d wrote %d bytes\n",iminor(filp->f_path.dentry->d_inode),bytesWritten);
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
	msg1 = (char *)vmalloc(MAX);
	msg2 = (char *)vmalloc(MAX);
	while(total1 < MAX)
	{
		if(rear1 == MAX-1)
			rear1 = -1;	
		msg1[++rear1] = (char)((i%26) + 65);
		total1++;
		i++;
	}
	if(major < 0)
	{
		printk(KERN_INFO "Can't Register Device %s \n",DEVICE_NAME);
		return -EFAULT;
	}
	printk(KERN_INFO "Loading the module : iodriver with device id:%d with string %s\n",major,msg1);

	return 0;
}

static void __exit remove_module(void)
{
	printk(KERN_INFO "Removing the module : iodriver with string : %s\n",msg2);
	unregister_chrdev(major,DEVICE_NAME);
}

module_init(load_module);
module_exit(remove_module);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Detamos");
MODULE_DESCRIPTION("Non-blocking read/write driver");
MODULE_SUPPORTED_DEVICE("iitpipe");
