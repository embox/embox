/**
 * @file
 * @brief Tests for POSIX semaphore.
 *
 * @date 20.03.02
 * @author Weixuan XIAO
 */

#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <embox/test.h>
#include <pthread.h>
#include <util/err.h>

EMBOX_TEST_SUITE("posix/semaphore test");

#define SEMAPHORE_INIT_VALUE 5

#define ts_cmp(a, b, CMP)               \
    (((a)->tv_sec == (b)->tv_sec) ?		\
     ((a)->tv_nsec CMP (b)->tv_nsec) :	\
     ((a)->tv_sec CMP (b)->tv_sec))

TEST_CASE("Test sem_init with shared=0, value=0") {
    sem_t semaphore;

    test_assert_equal(sem_init(&semaphore, 0, 0), ENOERR);
}

TEST_CASE("Test sem_init with shared=1, value=0") {
    sem_t semaphore;

    test_assert_equal(sem_init(&semaphore, 1, 0), ENOERR);
}

TEST_CASE("Test sem_init with shared=0, value=1") {
    sem_t semaphore;

    test_assert_equal(sem_init(&semaphore, 0, 1), ENOERR);
}

TEST_CASE("Test sem_init with shared=1, value=1") {
    sem_t semaphore;

    test_assert_equal(sem_init(&semaphore, 1, 1), ENOERR);
}

TEST_CASE("Test sem_destroy") {
    sem_t semaphore;

    /* Up to now, sem_destroy does nothing but return ENOERR */
    test_assert_equal(sem_destroy(&semaphore), ENOERR);
}

TEST_CASE("Test sem_getvalue") {
    sem_t semaphore;
    int value = -1;

    test_assert_equal(sem_init(&semaphore, 0, SEMAPHORE_INIT_VALUE), ENOERR);
    test_assert_equal(value, -1);                       /* Test before getvalue */
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, SEMAPHORE_INIT_VALUE); /* Test after getvalue */
}

TEST_CASE("Test sem_wait") {
    sem_t semaphore;
    int value = -1;

    test_assert_equal(sem_init(&semaphore, 0, SEMAPHORE_INIT_VALUE), ENOERR);
    test_assert_equal(value, -1);
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, SEMAPHORE_INIT_VALUE); 	/* Test before sem_wait */

    test_assert_equal(sem_wait(&semaphore), ENOERR);
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, SEMAPHORE_INIT_VALUE - 1); /* Test after sem_wait */
}

TEST_CASE("Test sem_post") {
    sem_t semaphore;
    int value = -1;

    test_assert_equal(sem_init(&semaphore, 0, SEMAPHORE_INIT_VALUE), ENOERR);
    test_assert_equal(value, -1);
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, SEMAPHORE_INIT_VALUE); 	/* Test before sem_wait */

    test_assert_equal(sem_wait(&semaphore), ENOERR);
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, SEMAPHORE_INIT_VALUE - 1); /* Test after sem_wait */

    test_assert_equal(sem_post(&semaphore), ENOERR);
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, SEMAPHORE_INIT_VALUE); 	/* Test after sem_post */
}

TEST_CASE("Test sem_trywait") {
    sem_t semaphore;
    int value = -1;

    test_assert_equal(sem_init(&semaphore, 0, 1), ENOERR);
    test_assert_equal(value, -1);
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, 1);

    test_assert_equal(sem_trywait(&semaphore), ENOERR);
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, 0);

    test_assert_equal(sem_trywait(&semaphore), -EAGAIN);
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, 0);
}

TEST_CASE("Test sem_timedwait") {
    sem_t semaphore;
    struct timespec now, ts;
    int value = -1;

    test_assert_equal(sem_init(&semaphore, 0, 1), ENOERR);
    test_assert_equal(value, -1);
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, 1);

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 100000000;		/* Wait 0.1s */
    test_assert_equal(sem_timedwait(&semaphore, &ts), ENOERR);
    clock_gettime(CLOCK_REALTIME, &now);
    test_assert(ts_cmp(&ts, &now, >));	/* Assert ts is after now */
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, 0);

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 100000000;		/* Wait 0.1s */
    test_assert_equal(sem_timedwait(&semaphore, &ts), -ETIMEDOUT);
    clock_gettime(CLOCK_REALTIME, &now);
    test_assert(ts_cmp(&ts, &now, <=));
    test_assert_equal(sem_getvalue(&semaphore, &value), ENOERR);
    test_assert_equal(value, 0);
}

static void *sem_post_run(void *arg) {
    struct timespec now, ts;
    sem_t *semaphore_test_timedwait = (sem_t *)arg;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 100000000;

    do {
        clock_gettime(CLOCK_REALTIME, &now);
    } while (ts_cmp(&now, &ts, <));

    test_assert_equal(sem_post(semaphore_test_timedwait), ENOERR);

    return NULL;
}

TEST_CASE("Test sem_timedwait successful") {
    struct timespec ts;
    int value = -1;
    pthread_t sem_post_thread;
    sem_t semaphore_test_timedwait;

    test_assert_equal(sem_init(&semaphore_test_timedwait, 0, 1), ENOERR);
    test_assert_equal(value, -1);
    test_assert_equal(sem_getvalue(&semaphore_test_timedwait, &value), ENOERR);
    test_assert_equal(value, 1);

    test_assert_equal(sem_trywait(&semaphore_test_timedwait), ENOERR);

    /* Create thread and launch it to release semaphore after 0.1s */
    test_assert_zero(pthread_create(&sem_post_thread, NULL, sem_post_run, &semaphore_test_timedwait));

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += 200000000;		/* Wait 0.2s */
    test_assert_equal(sem_timedwait(&semaphore_test_timedwait, &ts), ENOERR);

    pthread_join(sem_post_thread, NULL);
}

/*
TODO: Not yet implemented:
extern int    sem_close(sem_t *);
extern sem_t *sem_open(const char *, int, ...);
extern int    sem_unlink(const char *);
*/
