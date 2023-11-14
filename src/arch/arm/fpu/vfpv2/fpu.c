/**
 * @file
 *
 * @date May 24, 2018
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <string.h>

#include <util/log.h>
#include <util/math.h>
#include <arm/fpu.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(vfp_init);

static int vfp_init(void) {
	uint32_t ctrl;
	uint32_t sid;

	/* Enable VFP extensions */
	asm volatile(".fpu vfpv2");
	asm volatile("FMRX %0, FPEXC" : "=r"(ctrl));
	asm volatile("FMXR FPEXC, %0" : : "r"(ctrl | VFP_FPEXC_EN));

	/* Print VFP info */
	asm volatile("FMRX %0, FPSID" : "=r"(sid));

	log_info("VPF info:\n"
	         "\t\t\t %s\n"
	         "\t\t\t Implementer =        0x%02x (%s)\n"
	         "\t\t\t Subarch:             VFPv%d\n"
	         "\t\t\t Part number =        0x%02x\n"
	         "\t\t\t Variant     =        0x%02x\n"
	         "\t\t\t Revision    =        0x%02x",
	    (sid >> 23) & 1 ? "Software FP emulation" : "Hardware FP support",
	    sid >> 24, ((sid >> 24) == 0x41) ? "ARM" : "Unknown",
	    ((sid >> 16) & 0x7F) + 1, (sid >> 8) & 0xFF, (sid >> 4) & 0xF,
	    sid & 0xF);

	/* Return to previos state */
	asm volatile("FMXR FPEXC, %0" : : "r"(ctrl));

	return 0;
}

int try_vfp_instructions(fpu_context_t *vfp) {
	int ret;

	ret = 0;
	if (!(vfp->fpexc & VFP_FPEXC_EN)) {
		vfp->fpexc |= VFP_FPEXC_EN;
		memset(&vfp->vfp_regs, 0, sizeof(vfp->vfp_regs));
		ret = 1;
	}

	return ret;
}
