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
	uint32_t mvfr0, fpccr;

	/* Enable FPU */
	/** FIXME Currently FPU is enabled permanently */
	REG32_STORE(FPU_CPACR, REG32_LOAD(FPU_CPACR) | (0xf << 20));

	/* Disable FPU context preservation/restoration on exception
	 * entry and exit, because we can guarantee every irq handler
	 * execute without using FPU */
	REG32_STORE(FPU_FPCCR, REG32_LOAD(FPU_FPCCR) & ~(0x3 << 30));

	mvfr0 = REG32_LOAD(FPU_MVFR0);
	fpccr = REG32_LOAD(FPU_FPCCR);

	log_info("FPv4/FPv5 info:\n"
	         "\t\t\t MVFR0 (Media and VFP Feature Register 0) = 0x%08x\n"
	         "\t\t\t\t Single precision support = %s\n"
	         "\t\t\t\t Double precision support = %s\n"
	         "\t\t\t\t Size of FP register bank = %s\n"
	         "\t\t\t MVFR1 (Media and VFP Feature Register 1) = 0x%08x\n"
	         "\n"
	         "\t\t\tFPU registers:\n"
	         "\t\t\t CPACR (Coprocessor Access Control Register)     = 0x%08x\n"
	         "\t\t\t FPCCR (Floating-point Context Control Register) = 0x%08x\n"
	         "\t\t\t\t Automatic hardware state preservation and restoration\n"
	         "\t\t\t\t on exception entry and exit       = %s\n"
	         "\t\t\t\t Automatic lazy state preservation = %s\n",
	    mvfr0, (((mvfr0 >> 4) & 0xf) == 0x2) ? "yes" : "no",
	    (((mvfr0 >> 8) & 0xf) == 0x0) ? "no" : "Unknown",
	    (((mvfr0 >> 0) & 0xf) == 0x1) ? "16 x 64-bit registers" : "Unknown",
	    REG32_LOAD(FPU_MVFR1), REG32_LOAD(FPU_CPACR), fpccr,
	    (fpccr & (1u << 31)) ? "yes" : "no",
	    (fpccr & (1u << 30)) ? "yes" : "no");

	return 0;
}
