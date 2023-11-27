/**
 * @file
 * @details Support FPv4 and FPv5 for ARMv7-M
 *
 * @date 13.06.2018
 * @author Alexander Kalmuk
 */

#include <stdint.h>

#include <arm/fpu.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>
#include <util/math.h>

EMBOX_UNIT_INIT(fpv5_init);

static int fpv5_init(void) {
	uint32_t __attribute__((unused)) mvfr0;
	uint32_t __attribute__((unused)) fpccr;

	/* Enable FPU */
	/** FIXME Currently FPU is enabled permanently */
	REG32_STORE(FPU_CPACR, REG32_LOAD(FPU_CPACR) | (0xf << 20));

	/* Disable FPU context preservation/restoration on exception
	 * entry and exit, because we can guarantee every irq handler
	 * execute without using FPU */
	REG32_STORE(FPU_FPCCR, REG32_LOAD(FPU_FPCCR) & ~(0x3 << 30));

	mvfr0 = REG32_LOAD(FPU_MVFR0);
	fpccr = REG32_LOAD(FPU_FPCCR);

	log_info("vfp: MVFR0 (Media and VFP Feature Register 0) = 0x%08x", mvfr0);
	log_info("vfp: Single precision support = %s",
	    (((mvfr0 >> 4) & 0xf) == 0x2) ? "yes" : "no");
	log_info("vfp: Double precision support = %s",
	    (((mvfr0 >> 8) & 0xf) == 0x0) ? "no" : "Unknown");
	log_info("vfp: Size of FP register bank = %s",
	    (((mvfr0 >> 0) & 0xf) == 0x1) ? "16 x 64-bit registers" : "Unknown");
	log_info("vfp: MVFR1 (Media and VFP Feature Register 1) = 0x%08x",
	    REG32_LOAD(FPU_MVFR1));
	log_info("vfp: CPACR (Coprocessor Access Control Register) = 0x%08x",
	    REG32_LOAD(FPU_CPACR));
	log_info("vfp: FPCCR (Floating-point Context Control Register) = 0x%08x",
	    fpccr);
	log_info("vfp: Automatic hardware state preservation and restoration:");
	log_info("vfp: on exception entry and exit = %s",
	    (fpccr & (1u << 31)) ? "yes" : "no");
	log_info("vfp: automatic lazy state preservation = %s",
	    (fpccr & (1u << 30)) ? "yes" : "no");

	return 0;
}
