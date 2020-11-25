#include "serial.h"

// int fd;//文件描述符
// int epid; //epoll标识符
// struct epoll_event event;
// struct epoll_event events[6];//事件集合

int Serial::openSerial(const char *cSerialName, const char *Bitrate)
{
    int iFd;

    struct termios opt; 

    iFd = open(cSerialName, O_RDWR | O_NOCTTY |O_NONBLOCK);
	//iFd = open(cSerialName, O_RDWR | O_NOCTTY | O_NDELAY);//阻塞 |O_RSYNC
    if(iFd < 0) {
        perror(cSerialName);
        return -1;
    }

    tcgetattr(iFd, &opt); 

	u_int32_t rate = atoi(Bitrate);
	switch(rate) {
		case 115200:
		cfsetispeed(&opt, B115200);
		cfsetospeed(&opt, B115200);
		break;
		case 1000000:
		cfsetispeed(&opt, B1000000);
		cfsetospeed(&opt, B1000000);
		break;
		case 3000000:
		cfsetispeed(&opt, B3000000);
		cfsetospeed(&opt, B3000000);
		break;
		default :
		printf("error uart bitrate!\n");
		exit(0);
		break;
	}

    
    /*
     * raw mode
     */
    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE   |   PARENB);
    opt.c_cflag   |=   CS8;

    /*
     * 'DATA_LEN' bytes can be read by serial
     */
    opt.c_cc[VMIN]   =   DATA_LEN;                                      
    opt.c_cc[VTIME]  =   150;

    if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
        return   -1;
    }


    return iFd;
}

