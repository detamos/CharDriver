#ifndef DEVIOCTL_H
#define DEVIOCTL_H

#include <linux/ioctl.h>

static int major;

#define DEVICE_NAME "iodriver"
const char MKNOD[] = "mknod";
const char RM[] = "rm";
const char SPACE = " ";
const DEVICE_FILE1 "/dev/iitpipe0";
const DEVICE_FILE2 "/dev/iitpipe1";

void create_devFiles(int );
void remove_devFiles(int );
char *atoi(int );

#endif