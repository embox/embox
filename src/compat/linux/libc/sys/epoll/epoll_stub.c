/**
 * @file
 *
 * @date 01.12.25
 * @author Anton Bondarev
 */

#include <sys/epoll.h>

int epoll_create(int size) {
    return 0;
}

int epoll_create1(int flags) {
    return 0;
}

int epoll_ctl (int __epfd, int __op, int __fd, struct epoll_event *__event) {
    return 0;
}

int epoll_wait (int __epfd, struct epoll_event *__events, int __maxevents, int __timeout) {
    return 0;
}