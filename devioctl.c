#include "devioctl.h"
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>

char *atoi(int num)
{
	char *str = kmalloc(10,GFP_KERNEL);
	int i = 0;
	while(num && i < 10)
	{
		str[i] = (char)((num%10) + '0');
		num /= 10;
		i++;
	}
	return str;
}

void create_devFiles(int major)
{
	char syscall[] = "WHAT";
	strcpy(syscall,MKNOD);
	strcat(syscall,SPACE);
	strcat(syscall,DEVICE_FILE1);
	strcat(syscall,SPACE);
	char maj[] = "WHY";
	maj = atoi(major);
	strcat(syscall,maj);
	printk(KERN_ALERT "%s\n",syscall);
}

void remove_devFiles(int major)
{

}
