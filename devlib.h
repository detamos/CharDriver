#ifndef DEVIOCTL_H
#define DEVIOCTL_H

#include <linux/ioctl.h>


#define DEVICE_NAME "iodriver"
#define MAX 	 100000
#define IOCTL_SET_DELAY _IOR(major,0,int)
#define IOCTL_GET_DELAY _IOR(major,1,int)

static int major;

#endif
