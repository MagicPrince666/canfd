#ifndef __EPOLL_H__
#define __EPOLL_H__

extern struct epoll_event ev, events[20];
extern int epfd;

int setnonblocking( int fd );
int epoll_add(int fd);
int epoll_del(int fd);

#endif
