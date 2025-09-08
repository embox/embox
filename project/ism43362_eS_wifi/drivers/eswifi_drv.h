/**
 * @file
 * @author Anton ondarev
 * @version
 * @date 08.09.2025
 */

#ifndef ESWIFI_DRV_H_
#define ESWIFI_DRV_H_

//#include <kernel/time/timer.h>
#include <pthread.h>

struct  sock;

struct eswifi_dev {
    int state;
    struct sock *sk;
    struct sock **out_sk;
    //struct sys_timer eswifi_timer;
    pthread_t eswifi_poll_thread;
};

extern struct eswifi_dev eswifi_dev;

#define ESWIFI_STATE_SERVER_START    0x10

#define ESWIFI_TIMER_TICK           1000 /* mS */

#endif /* ESWIFI_DRV_H_ */