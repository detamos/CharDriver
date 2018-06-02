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
#include <pthread.h>

char *buffer;
int len,Delay;
FILE *inFile,*outFile;
int front = 0,rear = -1,total = 0;
int status,noneed;

int power = 0;
pthread_mutex_t lock;

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

void print(char *s,int l)
{
	int i = 0;
	while(i < l)
	{
		printf("%c",s[i++]);
	}
	printf("\n");
}

void *func_read(void *arg)
{
	int file_desc = open("/dev/iitpipe0",O_RDWR);

	char temp[1];
	while(read(file_desc,temp,1) == 1)
	{
		pthread_mutex_lock(&lock);
		if(rear == MAX-1)
			rear = -1;
		buffer[++rear] = temp[0];
		total++;
		pthread_mutex_unlock(&lock);
	}

	close(file_desc);
}

void *func_write(void *arg)
{
	int file_desc = open("/dev/iitpipe1",O_RDWR);

	char temp[1];
	while(total == 0);
	do
	{	
		pthread_mutex_lock(&lock);
		if(front == MAX)
			front = 0;
		temp[0] = buffer[front++];
		total--;
		pthread_mutex_unlock(&lock);

	}while(write(file_desc,temp,1) == 1);

	close(file_desc);
}

int main()
{
	if(pthread_mutex_init(&lock,NULL) != 0)
	{
		printf("Mutex initiation failed\n");
		exit(-1);
	}
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

	pthread_t read_t,write_t;
	pthread_create(&read_t,NULL,func_read,NULL);
	pthread_create(&write_t,NULL,func_write,NULL);

	pthread_join(read_t,NULL);
	pthread_join(write_t,NULL);
	pthread_mutex_destroy(&lock);

	return 0;
}
