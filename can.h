#ifndef __CAN_H__
#define __CAN_H__

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <inttypes.h>

#include <iostream>

class Can
{
public:
    Can(std::string dev);
    ~Can();
    bool Init();
    int Read(uint8_t* buf, int len);
    int Write(uint8_t* buf, int len);

private:
    std::string can_dev_;
    int socket_can_; /* can raw socket */ 
};

#endif
