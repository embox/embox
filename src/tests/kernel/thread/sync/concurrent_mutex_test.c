/**
 * @file
 * @brief
 *
 * @date 23.10.2014
 * @author Vita Loginova
 */

#include <embox/test.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread.h>
#include <util/err.h>
#include <unistd.h>

static struct thread *t1, *t2;
static struct mutex m;

EMBOX_TEST_SUITE("Concurrent mutex test");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_launch(t1));
	test_assert_zero(thread_join(t1, NULL));
	test_assert_zero(thread_join(t2, NULL));
}

static void *run(void *arg) {
	if ((int)arg == 1)
	{
	test_assert_zero(thread_launch(t2));
	}
	for(int i = 0; i < 10; i++)
	{
		mutex_lock(&m);
		sleep(0);
		mutex_unlock(&m);
		sleep(0);
	}
	return NULL;
}

static int setup(void) {
	mutex_init(&m);

	t1 = thread_create(THREAD_FLAG_SUSPENDED, run, (void*)1);
	test_assert_zero(ptr2err(t1));

	t2 = thread_create(THREAD_FLAG_SUSPENDED, run, (void*)2);
	test_assert_zero(ptr2err(t2));
	return 0;
}
