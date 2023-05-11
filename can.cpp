#include <assert.h>
#include <iostream>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "can.h"
#include "epoll.h"

SocketCan::SocketCan(std::string dev)
    : can_dev_(dev),
      rx_ring_buffer_(1024)
{
    socket_can_ = -1;
}

SocketCan::~SocketCan(void)
{
    if (socket_can_ > 0) {
        close(socket_can_);
    }
}

bool SocketCan::Init()
{
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;
    int required_mtu;
    int mtu;

    required_mtu = sizeof(struct can_frame);

    /* open socket */
    if ((socket_can_ = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("socket");
        return false;
    }

    strncpy(ifr.ifr_name, can_dev_.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex            = if_nametoindex(ifr.ifr_name);
    if (!ifr.ifr_ifindex) {
        perror("if_nametoindex");
        return false;
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (required_mtu > (int)CAN_MTU) {

        /* check if the frame fits into the CAN netdevice */
        if (ioctl(socket_can_, SIOCGIFMTU, &ifr) < 0) {
            perror("SIOCGIFMTU");
            return false;
        }
        mtu = ifr.ifr_mtu;

        if (mtu != CANFD_MTU) {
            printf("CAN interface is not CAN FD capable - sorry.\n");
            return false;
        }

        int opt = 1;
        /* interface is ok - try to switch the socket into CAN FD mode */
        if (setsockopt(socket_can_, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
                       &opt, sizeof(opt))) {
            printf("error when enabling CAN FD support\n");
            return false;
        }

        /* ensure discrete CAN FD length values 0..8, 12, 16, 20, 24, 32, 64 */
        // frame.len = can_dlc2len(can_len2dlc(frame.len));
    }

    // setsockopt(socket_can_, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    if (bind(socket_can_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return false;
    }
    MY_EPOLL.EpollAdd(socket_can_, std::bind(&SocketCan::ReadCallback, this));
    return true;
}

int SocketCan::ReadCallback()
{
    uint8_t buf[64];
    int len = read(socket_can_, buf, sizeof(buf));
	if(len > 0) {
		rx_ring_buffer_.RingBufferIn(buf, len);
	}
    return len;
}

int SocketCan::ReadBuffer(uint8_t *buf, const int len)
{
    return rx_ring_buffer_.RingBufferOut(buf, len);
}

int SocketCan::SendBuffer(uint8_t *buf, const int len)
{
	struct can_frame frame;   //发送帧
    memset(&frame, 0 , sizeof(struct can_frame));
    frame.can_id = 0x2;
    frame.len = len;
	memcpy(frame.data, buf, len);
    return write(socket_can_, &frame, sizeof(struct can_frame));
}
