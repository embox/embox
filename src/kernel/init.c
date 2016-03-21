/**
 * @file
 * @brief The kernel initialization sequence.
 *
 * @date 21.03.09
 * @author Anton Bondarev
 * @author Alexey Fomin
 * @author Eldar Abusalimov
 */

#include <hal/arch.h>
#include <hal/ipl.h>
#include <drivers/diag.h>
#include <embox/runlevel.h>
#include <kernel/printk.h>

#if 1

#include <xen/features.h>
#include <stdint.h>
#include <xen/xen.h>
#include <hypercall-x86_32.h>
#include <xen/event_channel.h>
#include <xen/io/console.h>

struct pt_regs {
	long ebx;
	long ecx;
	long edx;
	long esi;
	long edi;
	long ebp;
	long eax;
	int  xds;
	int  xes;
	long orig_eax;
	long eip;
	int  xcs;
	long eflags;
	long esp;
	int  xss;
};

char stack[8192];

uint8_t xen_features[XENFEAT_NR_SUBMAPS * 32];

extern shared_info_t shared_info;
extern void handle_input(evtchn_port_t port, struct pt_regs * regs);

shared_info_t *HYPERVISOR_shared_info;
start_info_t * start_info_global;

static void kernel_init_xen(start_info_t * start_info);
static int init_xen(start_info_t * start_info);

void kernel_start_xen(start_info_t * start_info) {
	HYPERVISOR_update_va_mapping((unsigned long) &shared_info, 
			__pte(start_info->shared_info | 7),
			UVMF_INVLPG);

	start_info_global = start_info;
	HYPERVISOR_shared_info = &shared_info;
	
	kernel_init_xen(start_info);

	init_xen(start_info);

	while (1) {
		arch_idle();
	}
}

static void kernel_init_xen(start_info_t * start_info) {
	arch_init();

	ipl_init();

	diag_init();
}

static int init_xen(start_info_t * start_info) {
	int ret;
	const runlevel_nr_t target_level = RUNLEVEL_NRS_TOTAL - 1;

	printk("\nEmbox kernel start\n");

	ret = runlevel_set(target_level);

	return ret;
}

#else

static void kernel_init(void);
static int init(void);
extern int system_start(void);
/**
 * The setup of the system, the run level and execution of the idle function.
 */
void kernel_start(void) {

	kernel_init();

	init();

	system_start();

	while (1) {
		arch_idle();
	}
}


/**
 * The initialization functions are called to set up interrupts, perform
 * further memory configuration, initialization of drivers, devices.
 */
static void kernel_init(void) {
	arch_init();

	ipl_init();

	diag_init();
}

/**
 * Set the run level to the value of the file system and net level.
 * @return 0
 */
static int init(void) {
	int ret;
	const runlevel_nr_t target_level = RUNLEVEL_NRS_TOTAL - 1;

	printk("\nEmbox kernel start\n");

	ret = runlevel_set(target_level);

	return ret;
}
#endif
