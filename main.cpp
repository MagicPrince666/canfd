#include <arpa/inet.h>
#include <assert.h>
#include <bitset>
#include <errno.h>
#include <execinfo.h>
#include <fcntl.h> /*file control*/
#include <iostream>
#include <linux/input.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "can.h"
#include "epoll.h"

int main(int argc, char **argv)
{
    SocketCan can("can0");
    can.Init();
    uint8_t test[] = {0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a};
	struct can_frame frame;

    while (1) {
        can.SendBuffer(test, sizeof(test));
        int ret = can.ReadBuffer((uint8_t *)&frame, sizeof(frame));
        if (ret > 0) {
            printf("can bus recv from %d:", frame.can_id);
            for (uint32_t i = 0; i < frame.len; i++) {
                printf("%02x ", frame.data[i]);
            }
            printf("\n");
        }
        sleep(1);
    }

    return 0;
}
