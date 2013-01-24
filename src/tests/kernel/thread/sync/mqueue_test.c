/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#include <embox/test.h>
#include <kernel/thread/sync/mqueue.h>
#include <kernel/thread.h>

#define MESSAGE_COUNT 3

static struct thread *low, *high;
static struct mqueue mq;
const char *messages[MESSAGE_COUNT] =
	{"first message",	"another sending", "the last post"};

EMBOX_TEST_SUITE("Message queue test");

TEST_SETUP(setup);

TEST_CASE("General") {
	test_assert_zero(thread_launch(low));
	test_assert_zero(thread_launch(high));
	test_assert_zero(thread_join(low, NULL));
	test_assert_zero(thread_join(high, NULL));
}

static void *low_run(void *arg) {
	for (int i = 0; i < MESSAGE_COUNT; i++) {
		test_assert_zero(mqueue_send(&mq, messages[i], strlen(messages[i])+1));
	}

	return NULL;
}

static void *high_run(void *arg) {
	char buf[MQUEUE_SIZE];
	int len;

	for (int i = 0; i < MESSAGE_COUNT; i++) {
		len = mqueue_receive(&mq, buf);
		test_assert_equal(strlen(messages[i]) + 1, len);
		test_assert_zero(strcmp(buf, messages[i]));
	}

	return NULL;
}

static int setup(void) {
	thread_priority_t l = 10, h = 20;

	mqueue_init(&mq);
	test_assert_zero(thread_create(&low, THREAD_FLAG_SUSPENDED, low_run, NULL));
	test_assert_zero(thread_create(&high, THREAD_FLAG_SUSPENDED, high_run, NULL));
	test_assert_zero(thread_set_priority(low, l));
	test_assert_zero(thread_set_priority(high, h));
	return 0;
}
