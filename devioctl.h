#ifndef DEVIOCTL_H
#define DEVIOCTL_H

#include <linux/ioctl.h>

#define MAJOR_NUM 244
#define DEVICE_NAME "iodriver"
#define DEVICE_FILE1 "iitpipe0"
#define DEVICE_FILE2 "iitpipe1"
#define DEVICE_FILE "iitpipe"
#define DEVICE_AUTHOR "Detamos"
#define DEVICE_DESC "Non-blocking read/write driver"

#endif