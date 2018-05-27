#include "devioctl.h"
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>

void create_devFiles(int major)
{
	char syscall[] = "WHAT";
	strcpy(syscall,MKNOD);
	strcat(syscall,SPACE);
	strcat(syscall,DEVICE_FILE1);
	strcat(syscall,SPACE);

	char maj[20];
	int i = 0,num = major;
	while(num)
	{
		maj[i++] = (char)((num % 10) + '0');
		num /= 10;
	}
	maj[i] = '\0';
	
	strcat(syscall,maj);
	printk(KERN_ALERT "%s\n",syscall);
}

void remove_devFiles(int major)
{

}
