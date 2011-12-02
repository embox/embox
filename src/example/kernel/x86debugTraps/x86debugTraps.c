/**
 * @file
 *
 * @brief
 *
 * @date 16.08.2011
 * @author Anton Bondarev
 */
#include <types.h>
#include <errno.h>
#include <stdio.h>
#include <framework/example/self.h>
#include <kernel/thread/api.h>
#include <kernel/thread/event.h>
#include <kernel/thread/sched.h>
#include <unistd.h>
#include <assert.h>

#include <kernel/irq.h>
#include <kernel/printk.h>


#include <asm/flags.h>

/**
 * This macro is used to register this example at the system.
 */
EMBOX_EXAMPLE(run);

static struct event breakpointHappened;

static unsigned counter = 0;

static irq_nr_t intNo = -1;

static void* threadB(void* args) {
  while (1) {
    assert(0 == sleep(2));
    sched_sleep(&breakpointHappened);
    printf("counter = %d\n", counter);
  }
  return NULL;
}

static void setTraceFlagX86(void);
// static void clearTraceFlagX86(void);

//static irq_return_t int03Handler(irq_nr_t irq_nr, void *dev_id) {
void int0301HandlerHack(irq_nr_t n) {
	// XXX clock_hander is called from arch part
   if (n == 3) {
        printk("int %d\n", n);
	intNo = n;
	assert(__cur_ipl == 0);
	sched_wake(&breakpointHappened);
   } else
	++ counter;

//	return IRQ_HANDLED;
}

static void* threadA(void* args) {
  printf("thread-counter started\n");
  setTraceFlagX86();
//  clearTraceFlagX86();
  while (1) {
	asm (".byte 0xCC;");
//	asm (" int $1; ");
	assert(0 == sleep(1));
  }
  return NULL;
}

/**
 * Example's executing routine
 * It has been declared by the macro EMBOX_EXAMPLE
 */
static int run(int argc, char **argv) {
	struct thread *thr[2];
	void *ret;
	int i;

	printf("Start thread's example\n");

//	assert(0 == irq_attach(3, int03Handler, 0, NULL, "int 03 debug handler"));
	event_init(&breakpointHappened, "breakpointHappened");

	assert(0 == thread_create(&thr[0], 0, threadA, NULL));
	assert(0 == thread_create(&thr[1], 0, threadB, NULL));

	/* waiting until all threads finish and print return value*/
	for(i = 0; i < 2; i ++) {
		thread_join(thr[i], &ret);
		printf("finished thread id %d with result %d\n", i, *((int *)ret));
	}
	printf("Finish thread's example\n");

	return ENOERR;
}


static void setTraceFlagX86(void) {
  asm ("\
	pushf;     \
	pop  %%eax; \
	or   $0x00000100, %%eax /*X86_EFLAGS_TF*/; \
	push %%eax; \
        popf;  \
  " : : : "%eax");
}

#if 0
static void clearTraceFlagX86(void) {
  asm ("\
	pushf;     \
	pop  %%eax; \
	and  $0xfffffeff, %%eax /*X86_EFLAGS_TF*/; \
	push %%eax; \
        popf;  \
  " : : : "%eax");
}
#endif

