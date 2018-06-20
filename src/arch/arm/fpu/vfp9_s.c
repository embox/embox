/**
 * @file
 *
 * @date May 24, 2018
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <util/log.h>
#include <util/math.h>
#include <arm/fpu.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(vfp9_s_init);

static int vfp9_s_init(void) {
	uint32_t val;
	uint32_t sid;

	log_boot_start();

	/* Enable FPU extensions */
	asm volatile ("VMRS %0, FPEXC" : "=r" (val) :);

	asm volatile ("VMSR FPEXC, %0" : : "r" (val | 1 << 30));

	asm volatile ("VMRS %0, FPSID" : "=r" (sid));

	log_boot("VPF info:\n"
			"\t\t\t %s\n"
			"\t\t\t Implementer =        0x%02x (%s)\n"
			"\t\t\t Subarch:             VFPv%d\n"
			"\t\t\t Part number =        0x%02x\n"
			"\t\t\t Variant     =        0x%02x\n"
			"\t\t\t Revision    =        0x%02x",
			(sid >> 23) & 1 ? "Software FP emulation" : "Hardware FP support",
			sid >> 24, ((sid >> 24) == 0x41) ? "ARM" : "Unknown",
			((sid >> 16) & 0x7F) + 1,
			(sid >> 8) & 0xFF,
			(sid >> 4) & 0xF,
			sid & 0xF);

	log_boot_stop();

	asm volatile ("VMSR FPEXC, %0" : : "r" (val));

	return 0;
}

int try_vfp_instructions(void /*struct pt_regs_fpu *vfp */) {
	uint32_t fpexc;

	asm volatile ("VMRS %0, FPEXC" : "=r" (fpexc) :);
	if (fpexc & (1 << 30)) {
		return 0;
	}

	fpexc |= 1 << 30;
	asm volatile ("VMSR FPEXC, %0" : : "r" (fpexc));

	/* vfp->fpexc = fpexc; */

	return 1;
}
