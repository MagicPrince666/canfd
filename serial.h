#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>   
#include <limits.h> 
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/epoll.h> 
#include <string.h>

//#define MAXLEN 15

#define DATA_LEN                0xFF         

class Serial
{

public:
    int fd;//文件描述符
    int openSerial(const char *cSerialName, const char *Bitrate);

private:  

};

#endif