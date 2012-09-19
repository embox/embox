/**
 * @file
 * @brief Test softirq subsystem
 *
 * @date 19.11.10
 * @author Sergey Knolodilov
 */
#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>
#include <unistd.h>
#include <kernel/softirq.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

/* softirq numbers, that will be reseted by test */
#define SOFTIRQ_HELLO_NUMBER      10
#define SOFTIRQ_REQURSION_NUMBER  11

/*
 * Data should be zero terminated char*
 * Output string and set global flag
 */
static void handler_hello(unsigned int nt, void* data) {
	printf("softirq #%d, data: %s\n", nt, (char*) data);
}

/*
 * Data should be int*.
 * Raise itself several times
 */
static void handler_recursion(unsigned int nt, void* data) {
	int* count = (int*) data;

	printf("softirq #%d, data: %d\n", nt, *count);
	*count -= 1;
	if(*count > 0) {
		softirq_raise(nt);
	}
}

int example_hello(void) {
	if(softirq_install(SOFTIRQ_HELLO_NUMBER, handler_hello, "hello softirq!")) {
		printf("Can not install softirq #%d\n", SOFTIRQ_HELLO_NUMBER);
		return -EBUSY;
	}

	softirq_raise(SOFTIRQ_HELLO_NUMBER);
	return ENOERR;
}

int example_recursion(int cnt) {
	int count = 0;

	if(softirq_install(SOFTIRQ_REQURSION_NUMBER, handler_recursion, &count)) {
		printf("Can not install softirq #%d\n", SOFTIRQ_REQURSION_NUMBER);
		return -EBUSY;
	}
	count = cnt;
	softirq_raise(SOFTIRQ_REQURSION_NUMBER);

	sleep(1);

	return ENOERR;
}

/**
 * Example's execution routine
 * It has been declared with macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	example_hello();
	example_recursion(5);
	return ENOERR;
}
