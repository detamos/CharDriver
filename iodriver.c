#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "devioctl.h"

static ssize_t device_read(struct file *filp,char *buffer,size_t len,loff_t *offset){ return 0; }
static ssize_t device_write(struct file *filp,const char *buffer,size_t len,loff_t *offset){ return 0; }

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
	unregister_chrdev(major,DEVICE_NAME);

	printk(KERN_ALERT "Removing the module : iodriver\n");
}

void create_devFiles(int major)
{
	char syscall[50];
	snprintf(syscall,50,"mknod /dev/iitpipe0 c %d 0",major);
	printk(KERN_ALERT "Creating device files : %s\n",syscall);
	
	char *argv[] = {"/bin/bash",syscall,NULL};
	static char *envp[] = {"HOME=/",NULL};
	if(!call_usermodehelper(argv[0],argv,envp,UMH_WAIT_EXEC))
	{
		printk(KERN_ALERT "Couldnt create device file\n");
		return ;
	}
}

void remove_devFiles(int major)
{
	char sys0[] = "rm /dev/iitpipe0";
	char sys1[] = "rm /dev/iitpipe1";
	printk(KERN_ALERT "Removing device files : %s\n",sys0);
	printk(KERN_ALERT "Removing device files : %s\n",sys1);
}

module_init(load_module);
module_exit(remove_module);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Detamos");
MODULE_DESCRIPTION("Non-blocking read/write driver");
MODULE_SUPPORTED_DEVICE("iitpipe");
