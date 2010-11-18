/**
 * @file
 * @brief Test softirq subsistem
 *
 * @date 19.11.2010
 * @author Sergey Knolodilov
 */

#include <kernel/softirq.h>
#include <stdio.h>
#include <embox/test.h>

EMBOX_TEST(run);

/* softirq numbers, that will be reseted by test */
#define HANDLER1_SOFTIRQ 10
#define HANDLER2_SOFTIRQ 11

volatile static int flag1 = 0;

/*
 * Data should be zero terminated char*
 * Output string and set global flag
 */
static void handler1(softirq_nr_t nt, void* data) {
	TRACE("softirq #%d, data: %s\n", nt, (char*)data);
	flag1 = 1;
}

/*
 * Data should be int*.
 * Raise itself several times
 */
static void handler2(softirq_nr_t nt, void* data) {
	int* count = (int*)data;

	TRACE("softirq #%d, data: %d\n", nt, *count);

	*count += 1;

	if (*count < 5)
		softirq_raise(nt);
}

/**
 * The test itself. Will reset 10 and 11 softirqs!
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int err = 0;
	volatile static int flag2 = 0;

	flag1 = 0;
	flag2 = 0;

	err = softirq_install(HANDLER1_SOFTIRQ, handler1, "hello!");

	if (err) {
		TRACE("Can not install softirq #%d, error %d", HANDLER1_SOFTIRQ, err);
		return 1;
	}

	softirq_install(HANDLER2_SOFTIRQ, handler2, &flag2);

	if (err) {
		TRACE("Can not install softirq #%d, error %d", HANDLER2_SOFTIRQ, err);
		return 1;
	}

	softirq_raise(HANDLER1_SOFTIRQ);
	softirq_raise(HANDLER2_SOFTIRQ);

	/*
	 * test fall with sleep(0)
	 * handlers called correctly, but for this function values of both flag1 and flag2
	 * stay 0, in spite of volatile. I don't know why, may be some compiler error.
	 */

	sleep(1);

	if ((flag1 == 0) || (flag2 == 0)) {
		TRACE("bad results: falg1 = %d, flag2 = %d", flag1, flag2);
		return 1;
	}

	return 0;
}
