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
    //int overrun_count;
    //timer_t  *tidp;

    //tidp = si->si_value.sival_ptr;
    //printf("Timer expired\n");

    //overrun_count = timer_getoverrun(*tidp);
    //printf("Timer expired overrun (%d)\n", overrun_count);
    //signal(sig, SIG_IGN);
    test_emit('0');
}

TEST_CASE("time/call timer_handler") {
    timer_t timerid;
    struct sigevent sev;
    struct itimerspec its;
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    test_assert(-1 != sigaction(SIGRTMIN, &sa, NULL));

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    test_assert(-1 != timer_create(CLOCK_REALTIME, &sev, &timerid));

    /* Arm the timer (start 10 ms, repeat every 10 ms) */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 100000000; /* 10 ms */
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 100000000;  /* 10 ms */
    test_assert(-1 != timer_settime(timerid, 0, &its, NULL));

    sleep(1);

    test_assert_emitted("0");

    timer_delete(timerid);

}

TEST_CASE("time/sigprocmask") {
    timer_t timerid;
    sigset_t mask;
    struct sigevent sev;
    struct itimerspec its;
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    test_assert(-1 != sigaction(SIGRTMIN, &sa, NULL));

    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    test_assert(-1 != sigprocmask(SIG_SETMASK, &mask, NULL));

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    test_assert(-1 != timer_create(CLOCK_REALTIME, &sev, &timerid));

    /* Arm the timer (start 10 ms, repeat every 10 ms) */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 100000000; /* 10 ms */
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 100000000;  /* 10 ms */
    test_assert(-1 != timer_settime(timerid, 0, &its, NULL));

    sleep(1);

    test_assert_emitted("0");
    
    test_assert(-1 != sigprocmask(SIG_UNBLOCK, &mask, NULL));

    test_assert(-1 != timer_delete(timerid));

}
