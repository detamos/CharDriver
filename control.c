#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "devioctl.h"
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void ioctl_change_delay(int file_desc,int delay)
{
	int ret;
	ret = ioctl(file_desc,IOCTL_SET_DELAY,delay);
	if(ret < 0)
	{
		printf("ioctl call failed : %d\n",ret);
		exit(-1);
	}
}

char *buffer;
int len,delay;

int main()
{
	buffer = NULL;
	printf("Enter the size for buffer (in bytes) : ");
	scanf("%d",&len);
	printf("Enter the delay (in ms) : ");
	scanf("%d",&delay);
	buffer = (char*)malloc(len);
	if(buffer == NULL)
	{
		printf("Can't allocate space for buffer\n");
		exit(-1);
	}

	pid_t pid = fork();

	if(pid < 0)
	{
		printf("Can't create new process. Exit");
		exit(-1);
	}

	int file_desc;
	if(pid == 0)
	{
		file_desc = open("iitpipe0",0);
		if(file_desc < 0)
		{
			printf("Can't open device file /dev/iitpipe0\n");
			exit(-1);
		}
		close(file_desc);
	}
	else
	{
		file_desc = open("iitpipe1",0);
		if(file_desc < 0)
		{
			printf("Can't open device file /dev/iitpipe1\n");
			exit(-1);
		}
		close(file_desc);
	}

	return 0;
}
