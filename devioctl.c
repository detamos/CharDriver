#include "devioctl.h"
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

void create_devFiles(int major)
{
	char syscall[] = "mknod /dev/iitpipe0";
	printk(KERN_ALERT "Creating device files : %s\n",syscall);
}

void remove_devFiles(int major)
{

}
