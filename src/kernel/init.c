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
#include <prom/diag.h>
#include <embox/runlevel.h>
#include <prom/prom_printf.h>

static void kernel_init(void);
static int init(void);

/**
 * The setup of the system, the run level and execution of the idle function.
 */
void kernel_start(void) {

	kernel_init();

	init();

	while (1) {
		arch_idle();
	}
}

/*#define FP_SEG(x) (uint16_t)((uint32_t)(x) >> 16)
#define FP_OFF(x) (uint16_t)((uint32_t)(x))

#define V_PX_HORIZ 800
#define V_H_SYNC_ST
#define V_H_SYNC_END
#define V_SC_LINES 600
#define V_V_SYNC_ST
#define V_V_SYNC_END
#define V_FLAGS 0x0
#define V_PX_CLOCK 0x0
#define V_REF_RATE (V_PX_CLOCK / V_PX_HORIZ / V_SC_LINES)

struct vesa_data_st {
	uint16_t wd;
	uint16_t h_sync_start;
	uint16_t h_sync_end;
	uint16_t ht;
	uint16_t v_sync_start;
	uint16_t v_sync_end;
	uint8_t  flags;
	uint32_t freq;
	uint16_t ref_rate;
};

union vesa_data {
	struct vesa_data_st st;
	char data[64];
};

static union vesa_data data;*/
/**
 * The initialization functions are called to set up interrupts, perform
 * further memory configuration, initialization of drivers, devices.
 */
static void kernel_init(void) {
/*	uint16_t es;
	uint16_t di;
	es = FP_SEG(data);
	di = FP_OFF(data);
	asm (
		"movw $0x4f02, %%ax\n\t"
		"movw $0x0115, %%bx\n\t"
		"movw %0, %%es\n\t"
		"int $0x10\n\t"
		: : "r"(es), "D"(di) : "%ax", "%bx"
	);
	while(1);*/
	arch_init();

	diag_init();

	ipl_init();
}

/**
 * Set the run level to the value of the file system and net level.
 * @return 0
 */
static int init(void) {
	int ret = 0;
	const runlevel_nr_t target_level = RUNLEVEL_NRS_TOTAL - 1;

	prom_printf("\nEmbox kernel start\n");

	if (0 != (ret = runlevel_set(target_level))) {
		prom_printf("Failed to get into level %d, current level %d\n",
				target_level, runlevel_get_entered());
	}
	return ret;
}
