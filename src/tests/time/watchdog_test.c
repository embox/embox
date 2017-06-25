/*
 * watchdog_test.c
 *
 *  Created on: 27th of march, 2014.
 *      Author: velib
 */
#include <embox/test.h>
#include <embox/unit.h>
#include <kernel/thread.h>
#include <kernel/time/time.h>
#include <kernel/time/watchdog.h>
#include <unistd.h>
#include <kernel/printk.h>


EMBOX_TEST_SUITE("Test_watchdog");


static struct timeval time_to_wait;
static void *thread_without_timeout(void *);
static void *thread_with_timeout(void * arg);
static void time_is_out(struct sys_timer *timer, void *param);
static watchdog_t w;
static int flag;

TEST_SETUP(wdog_setup);
TEST_TEARDOWN(wdog_teardown);

TEST_CASE("Checking working with no timeouts")
{
	struct thread * p_thread = NULL;
	p_thread = thread_create(0, &thread_without_timeout, NULL);
	thread_join(p_thread, NULL);
}


TEST_CASE("Checking working with timeout")
{
	struct thread * p_thread = NULL;
	p_thread = thread_create(0, &thread_with_timeout, NULL);
	thread_join(p_thread, NULL);
}


static int wdog_setup (void)
{
	time_to_wait.tv_sec = 2;
	time_to_wait.tv_usec = 0;
	flag = 0;

	watchdog_set(&w, &time_to_wait, &time_is_out);

	return 0;
}

static int wdog_teardown (void)
{
	watchdog_close(&w);

	return 0;
}


static void *thread_without_timeout(void * arg)
{
	watchdog_start(&w);
	sleep(1);
	watchdog_kick(&w);
	test_assert_zero(flag);

	return NULL;
}

static void *thread_with_timeout(void * arg)
{
	watchdog_start(&w);
	sleep(3);
	test_assert_not_zero(flag);

	return NULL;
}

static void time_is_out(struct sys_timer *timer, void *param) {
	flag = 1;
}

