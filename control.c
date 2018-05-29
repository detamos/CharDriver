#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "devlib.h"
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


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

	pid_t pid1 = fork();
	if(pid1 < 0)
	{
		printf("Can't fork new porcess(pid1). Terminating ...\n");
		exit(-1);
	}

	pid_t pid2 = fork();
	if(pid1 == 0)
	{
		if(pid2 < 0)
		{
			printf("Can't fork new porcess(pid2). Terminating ...\n");
			exit(-1);
		}
		if(pid2 == 0)
		{
			inFile = fopen("input","r");
			if(inFile == NULL)
			{
				printf("Can't open input file. Terminating ...\n");
				exit(-1);
			}

			char temp[1];
			int file_desc = open("/dev/iitpipe0",O_WRONLY);
			if(file_desc == -1)
			{
				printf("Can't open device file (pid1=0,pid2=0)\n");
				exit(-1);
			}

			while(fscanf(inFile,"%c",&temp[0]) == 1)
			{
				while(write(file_desc,temp,1) != 1);
			}
			fclose(inFile);
		}
		else
		{
			outFile = fopen("output","w");
			if(outFile == NULL)
			{
				printf("Can't open output file. Terminating ...\n");
				exit(-1);
			}

			int file_desc = open("/dev/iitpipe1",O_RDONLY);
			if(file_desc == -1)
			{
				printf("Can't open device file (pid1=0,pid2!=0)\n");
				exit(-1);
			}

			char temp[1];
			while(1)
			{
				if(read(file_desc,temp,1) != 1)
				{
					delay(10000);
					if(read(file_desc,temp,1) != 1)
						break;
				}
				fprintf(outFile,"%c",temp[0]);
			}

			fclose(outFile);
			close(file_desc);
		}
	}
	else
	{
		if(pid2 < 0)
		{
			printf("Can't fork new porcess(pid2(child)). Terminating ...\n");
			exit(-1);
		}
		if(pid2 == 0)
		{
			int file_desc = open("/dev/iitpipe0",O_RDONLY);
			if(file_desc == -1)
			{
				printf("Can't open device file (pid1=0,pid2!=0)\n");
				exit(-1);
			}

			char temp[1];
			while(1)
			{
				if(read(file_desc,temp,1) != 1)
				{
					delay(2000);
					if(read(file_desc,temp,1) != 1)
						break;
				}
				if(total == len)
					continue;
				if(rear == len-1)
				{
					rear = -1;
				}
				buffer[++rear] = temp[0];
				total ++;
			}

			close(file_desc);
		}
		else
		{
			int file_desc = open("/dev/iitpipe1",O_WRONLY);
			if(file_desc == -1)
			{
				printf("Can't open device file (pid1=0,pid2!=0)\n");
				exit(-1);
			}

			char temp[1];
			while(1)
			{
				if(total == 0)
				{
					delay(2000);
					if(total == 0)
						break;
				}
				temp[0] = buffer[front++];
				if(front == len)
					front = 0;
				total --;
				int written = write(file_desc,temp,1);
			}
		}
	}

	return 0;
}
