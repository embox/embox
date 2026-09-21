/**
 * @file
 * @brief CPU information back end for aarch64
 *
 * The ID registers describe the core the command runs on and no other: on a
 * big.LITTLE part MIDR_EL1 differs between clusters.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <hal/cpu.h>
#include <hal/reg.h>
#include <lib/libcpu_info.h>
#include <util/field.h>

static struct cpu_info current_cpu;

static const char *implementer_name(unsigned int code) {
	switch (code) {
	case 0x41:
		return "ARM";
	case 0x42:
		return "Broadcom";
	case 0x43:
		return "Cavium";
	case 0x4e:
		return "NVIDIA";
	case 0x51:
		return "Qualcomm";
	case 0x56:
		return "Marvell";
	case 0x61:
		return "Apple";
	case 0xc0:
		return "Ampere";
	default:
		return "unknown";
	}
}

static const char *arm_part_name(unsigned int part) {
	switch (part) {
	case 0xd03:
		return "Cortex-A53";
	case 0xd04:
		return "Cortex-A35";
	case 0xd05:
		return "Cortex-A55";
	case 0xd07:
		return "Cortex-A57";
	case 0xd08:
		return "Cortex-A72";
	case 0xd09:
		return "Cortex-A73";
	case 0xd0a:
		return "Cortex-A75";
	case 0xd0b:
		return "Cortex-A76";
	case 0xd0d:
		return "Cortex-A77";
	default:
		return "unknown part";
	}
}

static void set_part(struct cpu_info *info, uint64_t midr) {
	char buf[FEATURE_NAME_LEN];
	unsigned int implementer;
	unsigned int part;

	implementer = FIELD_GET(midr, MIDR_EL1_IMPLEMENTER);
	part = FIELD_GET(midr, MIDR_EL1_PARTNUM);

	if (implementer == 0x41) {
		set_feature_strval(info, "Part", arm_part_name(part));
	}
	else {
		snprintf(buf, sizeof(buf), "0x%03x", part);
		set_feature_strval(info, "Part", buf);
	}

	snprintf(buf, sizeof(buf), "r%up%u",
	    (unsigned int)FIELD_GET(midr, MIDR_EL1_VARIANT),
	    (unsigned int)FIELD_GET(midr, MIDR_EL1_REVISION));
	set_feature_strval(info, "Revision", buf);
}

static void set_mmu_features(struct cpu_info *info, uint64_t mmfr0) {
	/* Indexed by the PARange encoding */
	static const uint8_t pa_bits[] = {32, 36, 40, 42, 44, 48, 52, 56};
	char buf[FEATURE_NAME_LEN];
	unsigned int parange;

	parange = FIELD_GET(mmfr0, ID_AA64MMFR0_EL1_PAR);
	set_feature_val(info, "Physical address bits",
	    (parange < sizeof(pa_bits)) ? pa_bits[parange] : 0);

	/* What embox.arch.aarch64.mmu(granule=) may be set to on this part */
	buf[0] = '\0';
	if (FIELD_GET(mmfr0, ID_AA64MMFR0_EL1_TGRAN4)
	    != ID_AA64MMFR0_EL1_TGRAN4_NONE) {
		snprintf(buf, sizeof(buf), "4K ");
	}
	if (FIELD_GET(mmfr0, ID_AA64MMFR0_EL1_TGRAN16)
	    != ID_AA64MMFR0_EL1_TGRAN16_NONE) {
		snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "16K ");
	}
	if (FIELD_GET(mmfr0, ID_AA64MMFR0_EL1_TGRAN64)
	    != ID_AA64MMFR0_EL1_TGRAN64_NONE) {
		snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "64K");
	}
	set_feature_strval(info, "Page granules", buf[0] ? buf : "none");
}

struct cpu_info *get_cpu_info(void) {
	uint64_t midr;

	current_cpu.feature_count = 0;

	midr = ARCH_REG_LOAD(MIDR_EL1);
	strcpy(current_cpu.vendor_id,
	    implementer_name(FIELD_GET(midr, MIDR_EL1_IMPLEMENTER)));

	set_part(&current_cpu, midr);

	set_feature_val(&current_cpu, "Exception level",
	    FIELD_GET(ARCH_REG_LOAD(CurrentEL), CurrentEL_EL));

	set_mmu_features(&current_cpu, ARCH_REG_LOAD(ID_AA64MMFR0_EL1));

	set_feature_val(&current_cpu, "Counter frequency",
	    (unsigned int)ARCH_REG_LOAD(CNTFRQ_EL0));

	set_feature_val(&current_cpu, "CPU NUM", NCPU);

	return &current_cpu;
}

/* The generic timer's virtual count, fixed rate */
uint64_t get_cpu_counter(void) {
	/* The read is not ordered against the instructions around it */
	__asm__ __volatile__("isb" : : : "memory");

	return ARCH_REG_LOAD(CNTVCT_EL0);
}
