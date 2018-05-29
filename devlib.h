#ifndef DEVIOCTL_H
#define DEVIOCTL_H

#include <linux/ioctl.h>


#define DEVICE_NAME "iodriver"
#define MAX 		1000000
#define IOCTL_SET_DELAY _IOR(major,0,int)

#endif