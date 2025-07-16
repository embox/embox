/**
 * @file
 *
 * @author Anton Bondarev
 * @date 13.06.24
 */

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <drivers/gpio.h>

#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/sched.h>

#include <framework/mod/options.h>

#define IN_PORT       OPTION_GET(NUMBER,in_port)
#define IN_PIN_MASK  (1 << OPTION_GET(NUMBER,in_pin))
#define OUT_PORT      OPTION_GET(NUMBER,out_port)
#define OUT_PIN_MASK (1 << OPTION_GET(NUMBER,out_pin))

struct arguments {
	volatile int done;
	pthread_t work_thread;
};

#define STATUS_NONE       0
#define STATUS_WAIT       1
#define STATUS_HAPPENED    2
#define STATUS_FINISHED   3

static void irq_hnd(void *arg) {
	struct arguments *a = arg;

	if (a->done != STATUS_WAIT) {
		/* work_routine has not started yet */
		return;
	}

	a->done = STATUS_HAPPENED;

	sched_wakeup(&a->work_thread->schedee);
}

static void *work_routine(void *arg) {
	struct arguments *a = arg;

	gpio_set(OUT_PORT, OUT_PIN_MASK, GPIO_PIN_LOW);
	usleep(200000);

	printf("Start waiting rising on port(%d) pin(%d)\n", 
						IN_PORT, OPTION_GET(NUMBER,in_pin));
	printf("Output is on port(%d) pin(%d)\n",
						OUT_PORT, OPTION_GET(NUMBER,out_pin));

	a->done = 1;

	SCHED_WAIT(a->done == STATUS_HAPPENED);

	gpio_set(OUT_PORT, OUT_PIN_MASK, GPIO_PIN_HIGH);

	usleep(200000); /* 0.2 Sec */

	a->done = STATUS_FINISHED;

	return NULL;
}

int main(void) {
	struct arguments args;
	int prio;
	int res;
	int policy;
	struct sched_param par;

	args.done = 0;

	gpio_setup_mode(IN_PORT, IN_PIN_MASK, GPIO_MODE_INT_MODE_RISING);
	if (0 > gpio_irq_attach(IN_PORT, IN_PIN_MASK, irq_hnd, (void *)&args)) {
		printf("couldn't attach IRQ for port(%d) pin(%d)\n",
							IN_PORT, OPTION_GET(NUMBER,in_pin) );
		return -EBUSY;
	}

	gpio_setup_mode(OUT_PORT, OUT_PIN_MASK, GPIO_MODE_OUT);
	gpio_set(OUT_PORT, OUT_PIN_MASK, GPIO_PIN_LOW);
	usleep(200000);

	res = pthread_create(&args.work_thread, 0, work_routine, &args);
	if (res) {
		printf("couldn't create a thread\n" );
		return -EBUSY;
	}

	pthread_getschedparam(pthread_self(), &policy, &par);
	prio = par.sched_priority;
	prio++;
	pthread_setschedprio(args.work_thread, prio);
	pthread_detach(args.work_thread);

	while(args.done != STATUS_FINISHED) {
	};

	usleep(200000); /* 0.2 Sec */
	gpio_set(OUT_PORT, OUT_PIN_MASK, GPIO_PIN_LOW);

	printf("Test finished\n");
	gpio_irq_detach(IN_PORT, IN_PIN_MASK);

	return 0;
}
