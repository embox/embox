/*
 * watchdog_example.c
 *
 *  Created on: 27 марта 2014 г.
 *      Author: velib
 */
#include <embox/test.h>
#include <embox/unit.h>
#include <kernel/thread.h>
#include <kernel/softirq_lock.h>

#include <kernel/time/watchdog.h>
#include <unistd.h>
#include <kernel/printk.h>
#include <kernel/time/time.h>


EMBOX_TEST_SUITE("Test_watchdog");


static struct timeval time_to_wait;
static void *thread_without_timeout(void *);
static void *thread_with_timeout(void * arg);
static watchdog_t w;

TEST_SETUP(wdog_setup);
TEST_TEARDOWN(wdog_teardown);


TEST_CASE("Checking working with no timeouts")
{
	struct thread * p_thread = NULL;
	printk("\nHello!\n");
	p_thread = thread_create(0, &thread_without_timeout, NULL);
	thread_join(p_thread, NULL);
}


TEST_CASE("Checking working with timeout")
{
	struct thread * p_thread = NULL;
	printk("\nHello!\n");
	p_thread = thread_create(0, &thread_with_timeout, NULL);
	thread_join(p_thread, NULL);
}


static int wdog_setup (void)
{
	time_to_wait.tv_sec = 2;
	time_to_wait.tv_usec = 0;

	watchdog_set(&w, &time_to_wait);

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

	return NULL;
}

static void *thread_with_timeout(void * arg)
{
	watchdog_start(&w);
	sleep(3);

	/*
	watchdog_kick(&w);
	*/

	return NULL;
}
