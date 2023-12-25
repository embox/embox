/**
 * @file
 *
 * @date May 24, 2018
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <string.h>

#include <arm/fpu.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <util/log.h>
#include <util/math.h>

EMBOX_UNIT_INIT(vfp_init);

static int vfp_init(void) {
	uint32_t ctrl;
	uint32_t sid;

	/* Enable VFP extensions */
	__asm__ __volatile__("VMRS %0, FPEXC" : "=r"(ctrl));
	__asm__ __volatile__("VMSR FPEXC, %0" : : "r"(ctrl | VFP_FPEXC_EN));

	/* Print VFP info */
	__asm__ __volatile__("VMRS %0, FPSID" : "=r"(sid));

	log_info("%s",
	    ((sid >> 23) & 0x1) ? "software FP emulation" : "hardware FP support");
	log_info("implementer = %s", ((sid >> 24) == 0x41) ? "ARM" : "Unknown");
	log_info("subarch     = VFPv%d", ((sid >> 16) & 0x7F) + 1);
	log_info("part number = 0x%02x", (sid >> 8) & 0xFF);
	log_info("variant     = 0x%02x", (sid >> 4) & 0xF);
	log_info("revision    = 0x%02x", sid & 0xF);

	/* Return to previos state */
	__asm__ __volatile__("VMSR FPEXC, %0" : : "r"(ctrl));

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
