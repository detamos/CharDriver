#ifndef DEVIOCTL_H
#define DEVIOCTL_H

#include <linux/ioctl.h>

static int major;

#define DEVICE_NAME "iodriver"
#define IOCTL_SET_DELAY _IOR(major,0,int)

#endif