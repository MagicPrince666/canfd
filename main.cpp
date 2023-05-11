#include <stdio.h>
#include <fcntl.h>      /*file control*/
#include <signal.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <assert.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <execinfo.h>
#include <time.h>
#include <linux/input.h>
#include <sys/timerfd.h>
#include <bitset>

#include "can.h"
#include "epoll.h"

#define BUF_SIZE 1472

static void sigint_handler(int sig)
{
	std::cout << "--- quit the loop! ---" << std::endl;
	exit(0);
}

int main(int argc,char **argv)
{
	signal(SIGINT, sigint_handler);//信号处理

	uint8_t* buf = new uint8_t[BUF_SIZE];
	uint8_t* retdata = new uint8_t[BUF_SIZE];

	delete[] buf;
	delete[] retdata;
	
	return 0;
}
