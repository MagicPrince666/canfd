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

inline void ResponseTo(int can_fd) {

    struct canfd_frame frame;   //发送帧
    memset(&frame, 0 , sizeof(struct canfd_frame));
    frame.can_id = 0x2;
    frame.len = 16;

    frame.data[0] = 0x5b;
    frame.data[1] = 0x5b;
    frame.data[2] = 0x1;
    frame.data[3] = 0x1;

    int len = write(can_fd, &frame, sizeof(struct canfd_frame));
    std::cout << "\033[34m[" << "reg Write " << len << " bytes ]\033[39m" << std::endl;
}

// 飞机状态,8 bytes
struct UAV_Status
{
    uint8_t is_takeoff; // 起飞状态
    int8_t roll; //整数，单位°，分辨率为1°
    int8_t pitch; //整数，单位°，分辨率为1°
    int8_t yaw; //整数，单位°，分辨率为2°
    uint8_t height_land; //飞机对地高度。整数，单位dm，分辨率1dm
    int8_t height_altitude; //飞机相对起飞点的高度。整数，单位dm，分辨率1dm
    uint8_t velocity_scalar; //飞机水平面内速度的模长。整数，单位dm/s
    uint8_t REV_02; //保留位
    UAV_Status():is_takeoff(0),roll(0),pitch(0),yaw(0),height_land(0),height_altitude(0),velocity_scalar(0),REV_02(0) {}
} __attribute__((packed));

struct Send_UAV_Status
{
    uint8_t head1; //5B
    uint8_t head2;//5B
    uint8_t Cmd;//0x04
    uint8_t Answer; //0，表示命令不需应答
    uint16_t  datalen;//8，
    uint32_t  sys_time;//0，系统时间
    uint32_t  checksum;  //飞机状态数据按字节累加
    uint16_t  REV_01; //保留位
    UAV_Status status;
    Send_UAV_Status():head1(0x5b),head2(0x5b),Cmd(0x04),Answer(0),datalen(8),sys_time(0),checksum(0),REV_01(0),status() {}
} __attribute__((packed));

inline void ResponseToclo(int can_fd) {

    Send_UAV_Status xxxag;
    struct canfd_frame frame[2];   //发送帧
    frame[0].can_id = 0x2;
    frame[0].len = 16;
    frame[1].can_id = 0x2;
    frame[1].len = 16;

    memcpy(frame[0].data, &xxxag, 16);
    memset(frame[1].data, 0 , 16);
    //memcpy(frame[1].data, &xxxag + 16, 8);

    int len = write(can_fd, &frame[0], sizeof(struct canfd_frame));
    //std::cout << "\033[34m[" << ".clud Write " << len << " bytes ]\033[39m" << std::endl;
    len = write(can_fd, &frame[1], sizeof(struct canfd_frame));
    //std::cout << "\033[34m[" << ".clud Write " << len << " bytes ]\033[39m" << std::endl;
}

int main(int argc,char **argv)
{
	signal(SIGINT, sigint_handler);//信号处理

	int nfds;
	uint8_t* buf = new uint8_t[BUF_SIZE];
	uint8_t* retdata = new uint8_t[BUF_SIZE];


	epfd = epoll_create( 256 );

	Can canfd;
	epoll_add(canfd.socket_can);
	bool sta = false;

	//for(int i = 0; i < 5; i++)
    //ResponseToclo(canfd.socket_can);
    uint32_t *data = NULL;
    struct canfd_frame *frame;
	while( 1 ) {
		nfds = epoll_wait( epfd, events, 20, 1000 );

		if ( nfds == -1 ) {
			perror( "epoll_wait" );
			break;
		}

		if ( nfds == 0 ) {
#if 0
			struct canfd_frame frame;   //发送帧
			frame.can_id = 0x2;
			frame.len = 64;

			for(int i = 0; i < frame.len; i++) {
				frame.data[i] = i;
			}
			int len = write(canfd.socket_can, &frame, sizeof(struct canfd_frame));
			std::cout << "\033[34m[" << "Write " << len << " bytes ]\033[39m" << std::endl;
			//canfd.Write(uint8_t*(&frame), sizeof(struct canfd_frame));
#endif
		}

		for (int i = 0; i < nfds; i++ ) {
			int rfd = events[i].data.fd; //准确获取哪个事件的描述符
			if ( events[i].events & EPOLLIN ) {
					int len = read(rfd, buf, BUF_SIZE);

					if(rfd == canfd.socket_can) {

                        frame = (struct canfd_frame *)buf;
                        data = (uint32_t *) (frame->data);

                        if(*data == 0x01015b5b) {
                            ResponseTo(canfd.socket_can);
                        } else {
                            //std::cout << "canfd:" << len << std::endl;
#if 0
                            for(int i = 0; i < len; i++) {
                                printf("%x ", buf[i]);
                            }
                            printf("\n");
#endif
                            ResponseToclo(canfd.socket_can);
                        }
					}
			}
		}
	}

	close(epfd);

	delete[] buf;
	delete[] retdata;
	
	return 0;
}
