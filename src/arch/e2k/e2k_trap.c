/**
 * @file
 * @brief
 *
 * @date Mar 13, 2018
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <assert.h>

#include <kernel/printk.h>

#include <kernel/irq.h>

#include <asm/ptrace.h>
#include <asm/trap_def.h>
#include <asm/cpu_regs_types.h>

#include <drivers/interrupt/lapic/regs.h>

extern void e2k_interrupt_handler(void) ;

#define GET_NR_TIRS(tir_hi)		((tir_hi >> 56) & 0xff)

#define E2K_EXP_INTERRUPT  (1UL << 32)

struct e2k_tir {
	uint64_t lo;
	uint64_t hi;
};

static void e2k_get_tirs(struct e2k_tir *tirs) {
	int nr_tirs = -1;

	log_debug("TIR regiters:\n");

	do {
		++nr_tirs;
		assert(nr_tirs < MAX_TIRs_NUM);

		tirs[nr_tirs].hi = E2K_GET_DSREG(tir.hi);
		tirs[nr_tirs].lo = E2K_GET_DSREG(tir.lo);
		log_debug("TIR%d: lo=0x%lx, hi=0x%lx\n",
			nr_tirs, tirs[nr_tirs].lo, tirs[nr_tirs].hi);
	} while(GET_NR_TIRS(tirs[nr_tirs].hi));

	E2K_SET_DSREG(tir.lo, tirs[nr_tirs].lo);
}
static void e2k_exception_fp_show(void) {
	e2k_pfpfr_t pfpfr;
	e2k_fpcr_t fpcr;
	e2k_fpsr_t fpsr;

	printk("float point exception\n");
	pfpfr.word = e2k_pfpfr_read();

	printk("pfpfr (0x%x)\n", pfpfr.word);
	printk("\tie : %d\n"
			"\tde : %d\n"
			"\tze : %d\n"
			"\toe : %d\n"
			"\tue : %d\n"
			"\tpe : %d\n"
			"\tim : %d\n"
			"\tdm : %d\n"
			"\tzm : %d\n"
			"\tom : %d\n"
			"\tum : %d\n"
			"\tpm : %d\n"
			"\trc : %d\n"
			"\tfz : %d\n"
			"\tdie : %d\n"
			"\tdde : %d\n"
			"\tdze : %d\n"
			"\tdoe : %d\n"
			"\tdue : %d\n"
			"\tdpe : %d\n",
			pfpfr.fields.ie, pfpfr.fields.de, pfpfr.fields.ze, pfpfr.fields.oe, pfpfr.fields.ue,
			pfpfr.fields.pe, pfpfr.fields.im, pfpfr.fields.dm, pfpfr.fields.zm, pfpfr.fields.om,
			pfpfr.fields.um, pfpfr.fields.pm, pfpfr.fields.rc, pfpfr.fields.fz, pfpfr.fields.die,
			pfpfr.fields.dde, pfpfr.fields.dze, pfpfr.fields.doe, pfpfr.fields.due,
			pfpfr.fields.dpe);

	fpcr.word = e2k_fpcr_read();
	printk("fpcr(0x%x):\n", fpcr.word);
	printk("\tim : %d\n"
		"\tdm : %d\n"
		"\tzm : %d\n"
		"\tom : %d\n"
		"\tum : %d\n"
		"\tpm : %d\n"
		"\tone1 : %d\n"
		"\tzero1 : %d\n"
		"\tpc : %d\n"
		"\trc : %d\n"
		"\tic : %d\n",
		fpcr.fields.im, fpcr.fields.dm, fpcr.fields.zm, fpcr.fields.om,
		fpcr.fields.um, fpcr.fields.pm, fpcr.fields.one1, fpcr.fields.zero1,
		fpcr.fields.pc, fpcr.fields.rc, fpcr.fields.ic);

	fpsr.word = e2k_fpsr_read();
	printk("fpsr(0x%x):\n", fpsr.word);
	printk("\tie : %d\n"
		"\tde : %d\n"
		"\tze : %d\n"
		"\toe : %d\n"
		"\tue : %d\n"
		"\tpe : %d\n"
		"\tzero1 : %d\n"
		"\tes : %d\n"
		"\tzero2 : %d\n"
		"\tc1 : %d\n"
		"\tzero3 : %d\n"
		"\tbf : %d\n",
		fpsr.fields.ie, fpsr.fields.de, fpsr.fields.ze, fpsr.fields.oe,
		fpsr.fields.ue, fpsr.fields.pe, fpsr.fields.zero1, fpsr.fields.es,
		fpsr.fields.zero2, fpsr.fields.c1, fpsr.fields.zero3, fpsr.fields.bf);
}

static void e2k_exception_handler(struct e2k_tir tir0) {
	printk("e2k_exception_handler:\n");
	printk("  TIRO: lo=0x%lx\n", tir0.lo);
	printk("      : hi=0x%lx (num=%ld, MOVA=0x%lx, ALS=0x%lx, exp=0x%lx)\n",
			tir0.hi,
			(tir0.hi >> 56) & 0xFF,
			(tir0.hi >> 52) & 0x0F,
			(tir0.hi >> 44) & 0x3F,
			(tir0.hi) & ((1UL << 44) - 1));
	switch((tir0.hi) & ((1UL << 44) - 1)) {
	case exc_fp_mask:
		e2k_exception_fp_show();
		break;
	default:
	}
}

void e2k_trap_handler(struct pt_regs *regs) {
	struct e2k_tir tirs[MAX_TIRs_NUM];

	e2k_get_tirs(tirs);

	if (tirs[0].hi & E2K_EXP_INTERRUPT) {
		e2k_interrupt_handler();
	} else {
		e2k_exception_handler(tirs[0]);
	}
}
