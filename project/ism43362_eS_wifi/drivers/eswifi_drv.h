/**
 * @file
 * @author Anton ondarev
 * @version
 * @date 08.09.2025
 */

#ifndef ESWIFI_DRV_H_
#define ESWIFI_DRV_H_

#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct  sock;
struct net_device ;

struct eswifi_dev {
    int state;
    struct net_device *netdev;
    char rx_buf[0x600];

    struct sock *sk;

    pthread_t eswifi_poll_thread;
    struct sockaddr_in src_in;

};

extern struct eswifi_dev eswifi_dev;

#define ESWIFI_STATE_SERVER_START    0x10

#define ESWIFI_TIMER_TICK           1000 /* mS */

#endif /* ESWIFI_DRV_H_ */