#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "devlib.h"
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>


void ioctl_set_delay(int file_desc,int delay)
{
	int ret;
	ret = ioctl(file_desc,IOCTL_SET_DELAY,delay);
	if(ret < 0)
	{
		printf("ioctl call failed : %d\n",ret);
		exit(-1);
	}
}

int ioctl_get_delay(int file_desc,int delay)
{
	int ret;
	ret = ioctl(file_desc,IOCTL_GET_DELAY,delay);
	if(ret < 0)
	{
		printf("ioctl call failed : %d\n",ret);
		exit(-1);
	}
	return ret;
}

void delay(double sec)
{
	clock_t start = clock();
	while(1)
	{
		clock_t end = clock();
		if((end-start)/CLOCKS_PER_SEC >= sec)
			break;
	}
}

char *buffer;
int len,Delay;
FILE *inFile,*outFile;
int front = 0,rear = -1,total = 0;
int status,noneed;

int power = 0;

void print(char *s,int l)
{
	int i = 0;
	while(i < l)
	{
		printf("%c",s[i++]);
	}
	printf("\n");
}

int main()
{
	buffer = NULL;
	printf("Enter the size for buffer (in bytes) : ");
	scanf("%d",&len);
	printf("Enter the delay (in ms) : ");
	scanf("%d",&Delay);
	buffer = (char*)malloc(len);
	if(buffer == NULL)
	{
		printf("Can't allocate space for buffer\n");
		exit(-1);
	}

	pid_t pid = fork();
	if(pid == 0)
	{
		int file_desc = open("/dev/iitpipe0",O_RDWR);
		char temp[1];
		int ctr = 0;
		while(read(file_desc,temp,1) == 1)
		{
			while(power);
			power = 1;
			if(rear == len - 1)
				rear = -1;
			buffer[++rear] = temp[0];
			total++;
			print(buffer,len);
			power = 0;
		}
		close(file_desc);
	}
	else
	{
		int file_desc = open("/dev/iitpipe1",O_RDWR);
		char temp[1];
		printf("Am i here ??\n");
		while(1)
		{
			printf("Total : %d\n",total);
			if(total == 0)
				continue;
			while(power);
			power = 1;
			temp[0] = buffer[front++];
			if(write(file_desc,temp,1) != 1)
				break;
			if(front == len)
				front = 0;
			total--;
			power = 0;
		}
		printf("Then why isnt it working ??\n");
		close(file_desc);
	}
	
	return 0;
}
