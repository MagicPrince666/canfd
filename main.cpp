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
#include "serial.h"

#define BUF_SIZE 1472

static void sigint_handler(int sig)
{
	std::cout << "--- quit the loop! ---" << std::endl;
	exit(0);
}

#define CAN_DEVICE "can0"

int main(int argc,char **argv)
{
	signal(SIGINT, sigint_handler);//信号处理

	int nfds;
	uint8_t* buf = new uint8_t[BUF_SIZE];
	uint8_t* retdata = new uint8_t[BUF_SIZE];


	epfd = epoll_create( 256 );

	Can canfd;
	epoll_add(canfd.socket_can);

	while( 1 ) {
		nfds = epoll_wait( epfd, events, 20, 1000 );

		if ( nfds == -1 ) {
			perror( "epoll_wait" );
			break;
		}

		if ( nfds == 0 ) {
			struct canfd_frame frame;   //发送帧
			frame.can_id = 0x2;
			frame.len = 64;

			for(int i = 0; i < frame.len; i++) {
				frame.data[i] = i;
			}
			int len = write(canfd.socket_can, &frame, sizeof(struct canfd_frame));
			std::cout << "\033[34m[" << "Write " << len << " bytes ]\033[39m" << std::endl;
			//canfd.Write(uint8_t*(&frame), sizeof(struct canfd_frame));
		}

		for (int i = 0; i < nfds; i++ ) {
			int rfd = events[i].data.fd; //准确获取哪个事件的描述符
			if ( events[i].events & EPOLLIN ) {
					int len = read(rfd, buf, BUF_SIZE);
					if(rfd == canfd.socket_can) {
						std::cout << "canfd:" << len << std::endl;
						for(int i = 0; i < len; i++) {
							printf("%x ", buf[i]);
						}
						printf("\n");
					}
			}
		}
	}

	close(epfd);

	delete[] buf;
	delete[] retdata;
	
	return 0;
}
