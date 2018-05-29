#ifndef DEVIOCTL_H
#define DEVIOCTL_H

#include <linux/ioctl.h>


#define DEVICE_NAME "iodriver"
#define MAX 		1000000
#define IOCTL_SET_DELAY _IOR(major,0,int)

static int major;
static int Device_open = 0;
static int Delay = 0;
static char msg[MAX];
static int front = 0;
static int rear = -1;
static int total = 0;
static char tempData;

#endif