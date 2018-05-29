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

void delay(unsigned long milli)
{
	clock_t start = clock();
	while(1)
	{
		clock_t end = clock();
		if((end-start) >= milli)
			break;
	}
}

char *buffer;
int len,Delay;
FILE *inFile,*outFile;
int front = 0,rear = -1,total = 0;
int status,noneed;


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
		if(file_desc < 0)
		{
			printf("Can't open device file /dev/iitpipe0\n");
			exit(-1);
		}

		FILE *inFile = NULL;
		inFile = fopen("input","r");
		char temp[1];
		int flag;
start_A :
		flag = 1;
		while(fscanf(inFile,"%c",&temp[0]) == 1 && flag == 1)
		{
			while(1)
			{
				int ret = write(file_desc,temp,1);
				if(ret == 0)
					continue;
				if(ret == 1)
					break;
				if(ret == -1)
				{
					flag = 0;
					break;
				}
			}
		}
		while(1)
		{
			int ret = read(file_desc,temp,1);
			if(ret == 0)
				break;
			if(total == len)
				continue;
			if(rear == len-1)
				rear = -1;
			buffer[++rear] = temp[0];
			total++;
		}
		if(flag == 0)
			goto start_A;
		fclose(inFile);
		close(file_desc);
	}
	else
	{
		int file_desc = open("/dev/iitpipe1",O_RDWR);
		if(file_desc < 0)
		{
			printf("Can't open device file /dev/iitpipe1\n");
			exit(-1);
		}

		FILE *outFile = NULL;
		outFile = fopen("output","w");

		char temp[1];
		int flag;
start_B:
		flag = 1;
		while(1)
		{
			if(total == 0)
				break;
			temp[0] = buffer[front++];
			if(front == len)
			{
				front = 0;
			}
			total--;

			int ret = write(file_desc,temp,1);
			if(ret == -1)
			{
				flag = 0;
				break;
			}
		}

		while(1)
		{
			int ret = read(file_desc,temp,1);
			if(ret == 0)
				break;
			fprintf(outFile,"%c",temp[0]);
		}

		if(flag == 0)
			goto start_B;

		fclose(outFile);
		close(file_desc);
	}

	return 0;
}
