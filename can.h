#ifndef __CAN_H__
#define __CAN_H__

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <inttypes.h>

#include <iostream>

#include "ring_buffer.h"

class SocketCan
{
public:
    SocketCan(std::string dev = "can0");
    ~SocketCan();

    bool Init();
    int ReadBuffer(uint8_t* buf, const int len);
    int SendBuffer(uint8_t* buf, const int len);

private:
    int ReadCallback();
    std::string can_dev_;
    int socket_can_;
    RingBuffer rx_ring_buffer_; // 接收缓存队列
};

#endif
