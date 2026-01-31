/**
 * @file
 *
 * @date 30.01,2026
 * @author Anton Bondarev
 */

#include <embox/test.h>

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

EMBOX_TEST_SUITE("time/timer_*");

static void timer_handler(int sig, siginfo_t *si, void *uc) {
    printf("Timer expired\n");
}

TEST_CASE("time/timer_") {
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, NULL);

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    timer_create(CLOCK_REALTIME, &sev, &timerid);

    /* Arm the timer (start 1s, repeat every 1s) */
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;
    timer_settime(timerid, 0, &its, NULL);

    sleep(5);

    timer_delete(timerid);

}
