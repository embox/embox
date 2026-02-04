/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    14.01.2026
 */

#include <embox/test.h>

#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

EMBOX_TEST_SUITE("signal/sigwait");

TEST_CASE("signal/sigwait ") {
    sigset_t sigset;
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec ts;
    int sig;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGRTMIN);
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    timer_create(CLOCK_REALTIME, &sev, &timerid);


    ts.it_value.tv_sec = 1;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 500000000;
    timer_settime(timerid, 0, &ts, NULL);

    // 4. Wait for timer

    while(1) {
        sigwait(&sigset, &sig);
        printf("Timer expired\n");
    }

}