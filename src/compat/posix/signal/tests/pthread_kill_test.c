/**
 * @file
 * @brief test for pthread_kill
 *
 * @author  XU Chenglin
 * @date    09/03/2020
 */

#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <embox/test.h>
#include <errno.h>

EMBOX_TEST_SUITE("posix/pthread_kill");

static void sighand_a(int signo) {
    test_assert_not_zero(signo);
    test_emit('a');
    return;
}

static void sighand_b(int signo) {
    test_assert_not_zero(signo);
    test_emit('b');
    return;
}

static void *thread_test(void *parm) {
    int rc = sleep(3);
    test_assert_not_zero(rc);
    return NULL;
}

static void *thread_ref(void *parm) {
    int rc = sleep(3);
    test_assert_zero(rc);
    return NULL;
}

static void *thread_immediate_return(void *parm) {
    return NULL;
}

TEST_CASE("Send signal to a running thread") {
    int rc;
    struct sigaction actions_a, actions_b;
    pthread_t thread_normal, thread_blind;

    sigemptyset(&actions_a.sa_mask);
    actions_a.sa_flags = 0;
    actions_a.sa_handler = sighand_a;
    sigemptyset(&actions_b.sa_mask);
    actions_b.sa_flags = 0;
    actions_b.sa_handler = sighand_b;
    rc = sigaction(SIGALRM, &actions_a, NULL);
    rc = sigaction(SIGINT, &actions_b, NULL);
    rc = pthread_create(&thread_normal, NULL, thread_test, NULL);
    if (rc != 0) {
        test_fail("Failed pthread_create\n");
    }
    rc = pthread_create(&thread_blind, NULL, thread_ref, NULL);
    if (rc != 0) {
        test_fail("Failed pthread_create\n");
    }
    rc = pthread_kill(thread_normal, SIGALRM);
    test_assert_zero(rc);
    rc = pthread_join(thread_normal, NULL);
    test_assert_emitted("a");
    rc = pthread_join(thread_blind, NULL);
}

TEST_CASE("Send signal 0 to a thread") {
    int rc;
    struct sigaction actions;
    pthread_t thread_normal;

    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = sighand_a;
    rc = sigaction(SIGALRM, &actions, NULL);
    rc = pthread_create(&thread_normal, NULL, thread_ref, NULL);
    if (rc != 0) {
        test_fail("Failed pthread_create\n");
    }
    rc = pthread_kill(thread_normal, 0);
    test_assert_zero(rc);
    rc = pthread_join(thread_normal, NULL);
    test_assert_emitted("");
}

TEST_CASE("Send signal to an unavailable thread") {
    int rc;
    struct sigaction actions;
    pthread_t thread_blank;

    actions.sa_flags = 0;
    actions.sa_handler = sighand_a;
    rc = sigaction(SIGALRM, &actions, NULL);
    rc = pthread_create(&thread_blank, NULL, thread_immediate_return, NULL);  
    if (rc != 0) {
        test_fail("Failed pthread_create\n");
    }
    rc = pthread_join(thread_blank, NULL);

    rc = pthread_kill(thread_blank, SIGALRM);
    test_assert_zero(rc);
}

TEST_CASE("Send an invalid signal to a thread") {
    int rc;
    struct sigaction actions;
    pthread_t thread_normal;

    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = sighand_a;
    rc = sigaction(SIGALRM, &actions, NULL);
    rc = pthread_create(&thread_normal, NULL, thread_ref, NULL);
    if (rc != 0) {
        test_fail("Failed pthread_create\n");
    }
    rc = pthread_kill(thread_normal, _SIG_TOTAL + 1);
    test_assert_not_zero(rc);
    rc = pthread_join(thread_normal, NULL);
    test_assert_emitted("");
}
