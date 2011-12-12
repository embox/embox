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

#include <asm/traps.h>
#include <asm/flags.h>

#include <sys/user.h>

/**
 * This macro is used to register this example at the system.
 */
EMBOX_EXAMPLE(run);

static struct event breakpointHappened;

static unsigned counter = 0;

static irq_nr_t intNo = -1;

static void* threadB(void* args) {
  while (1) {
    sched_sleep(&breakpointHappened);
    if (intNo == 3) printk("int03. counter = %d\n", counter);
    ++counter;
  }
  return NULL;
}

static void setTraceFlagX86(void);
// static void clearTraceFlagX86(void);

static struct user lastProcessorState;

//static irq_return_t int03Handler(irq_nr_t irq_nr, void *dev_id) {
void int0301HandlerHack(pt_regs_t* regs) {
  irq_nr_t n = regs -> trapno;

  struct user_regs_struct* r = &(lastProcessorState.regs);
  struct user* u = &lastProcessorState;

  r -> eflags = regs -> eflags;
  r -> eip = regs -> eip;
  r -> orig_eax = regs -> eax; // todo: don't know what does this mean!
  r -> eax = regs -> eax;
  r -> ebx = regs -> ebx;
  r -> ecx = regs -> ecx;
  r -> edx = regs -> edx;
  r -> esi = regs -> esi;
  r -> edi = regs -> edi;
  r -> ebp = regs -> ebp;
  r -> esp = regs -> esp2;

  r -> xcs = regs -> cs;
  r -> xds = regs -> ds;
  r -> xes = regs -> es;
  r -> xfs = regs -> fs;
  r -> xgs = regs -> gs;
  r -> xss = regs -> ss2;

  u -> signal = 0;
  u -> start_stack = (unsigned long)(thread_get_stack_start(thread_self()));

  // todo: critical_inside(0xFFFFFFFF) is not a very aesthetical way to say what i want?
  if (critical_inside(0xFFFFFFFF)) return;  // this debugger doesn't operate inside any kind of a critical context, cause it uses a separate thread

  intNo = n;
  sched_wake(&breakpointHappened);

/* TODO: HACK. Here we suppose that sched_wake doesn't return until the debugger thread wants to continue execution.
   This assumption may not always be true, though we have set the highest priority for the debug thread
   */

  regs -> eflags = r -> eflags;
  regs -> eip = r -> eip;
  regs -> eax = r -> eax;
  regs -> ebx = r -> ebx;
  regs -> ecx = r -> ecx;
  regs -> edx = r -> edx;
  regs -> esi = r -> esi;
  regs -> edi = r -> edi;
  regs -> ebp = r -> ebp;
  regs -> esp2 = r -> esp;
// must be done exactly when protection level is changed on return  regs -> esp = r -> esp;

  regs -> cs = r -> xcs;
  regs -> ds = r -> xds;
  regs -> es = r -> xes;
  regs -> fs = r -> xfs;
  regs -> gs = r -> xgs;
// must be done exactly when protection level is changed on return   regs -> ss = r -> xss;
  regs -> ss2 = r -> xss;

  return;
}

static void* threadA(void* args) {
  printf("thread-counter started\n");
  setTraceFlagX86();
//  clearTraceFlagX86();
  while (1) {
	asm (".byte 0xCC;");
//	asm (" int $1; ");
	assert(0 == sleep(1));
    printf("counterA = %d\n", counter);
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

	asm ( " mov %%cr0, %0 " :  "=r"(i) );
	printf("cr0 = %08x\n", i);

//	assert(0 == irq_attach(3, int03Handler, 0, NULL, "int 03 debug handler"));
	event_init(&breakpointHappened, "breakpointHappened");

	assert(0 == thread_create(&thr[0], 0, threadA, NULL));
	assert(0 == thread_create(&thr[1], 0, threadB, NULL));
	assert(0 == thread_set_priority(thr[1], THREAD_PRIORITY_MAX));

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

