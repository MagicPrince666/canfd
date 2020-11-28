#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include "epoll.h"

struct epoll_event ev, events[20];
int epfd;

int setnonblocking( int fd )
{
	int sta = fcntl( fd, F_GETFD, 0 ) | O_NONBLOCK;
	if ( fcntl( fd, F_SETFL, sta ) == -1 ){
		return -1;
	}
	return 0;
}

int epoll_add(int fd)
{
	ev.data.fd = fd;
	ev.events = EPOLLIN ;
	epoll_ctl( epfd, EPOLL_CTL_ADD, fd, &ev );
	return 0;
}

int epoll_del(int fd)
{
	ev.data.fd = fd;
	epoll_ctl( epfd, EPOLL_CTL_DEL, fd, NULL);
	return 0;
}
