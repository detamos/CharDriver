#include <devioctl.h>
#include <stdlib.h>

char *atoi(int num)
{
	char str[10];
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
	char *syscall;
	strcpy(syscall,MKNOD);
	strcat(syscall,space);
	strcat(syscall,DEVICE_FILE1);
	strcat(syscall,space);
	char *maj = atoi(major);
	strcat(syscall,maj);
	printk(KERN_ALERT "%s\n",syscall);
}

void remove_devFiles(int major)
{

}