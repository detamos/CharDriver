#ifndef DEVIOCTL_H
#define DEVIOCTL_H

#include <linux/ioctl.h>

static int major;

#define DEVICE_NAME "iodriver"

void create_devFiles(int );
void remove_devFiles(int );

#endif