/**
 * @file
 * @brief
 *
 * @date   26.04.2021
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <lib/libds/array.h>
#include <util/log.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(load_all_sections);

extern void *sections_text_vma[];
extern void *sections_text_lma[];
extern unsigned int sections_text_len[];

extern void *sections_data_vma[];
extern void *sections_data_lma[];
extern unsigned int sections_data_len[];

extern void *sections_rodata_vma[];
extern void *sections_rodata_lma[];
extern unsigned int sections_rodata_len[];

extern void *sections_bss_vma[];
extern void *sections_bss_lma[];
extern unsigned int sections_bss_len[];

/* Loads multiple sections. For example, all text sections. */
static void load_sections(void *vmas[], void *lmas[], unsigned int lens[]) {
	int i = 0;

	while (vmas[i]) {
		if (lens[i] && vmas[i] != lmas[i]) {
			log_debug("vma=%p, lma=%p, len=0x%x", vmas[i], lmas[i], lens[i]);
			memcpy(vmas[i], lmas[i], lens[i]);
		}
		i++;
	}
}

/* Zeroes multiple sections. For example, all bss sections. */
static void zero_sections(void *vmas[], unsigned int lens[]) {
	int i = 0;

	while (vmas[i]) {
		if (0 != lens[i]) {
			log_debug("vma=%p, len=0x%x", vmas[i], lens[i]);
			memset(vmas[i], 0, lens[i]);
		}
		i++;
	}
}

static int load_all_sections(void) {
	log_debug("");

	log_debug("Loading text sections:");
	load_sections(sections_text_vma, sections_text_lma, sections_text_len);

	log_debug("Loading rodata sections:");
	load_sections(sections_rodata_vma, sections_rodata_lma, sections_rodata_len);

	log_debug("Loading data sections:");
	load_sections(sections_data_vma, sections_data_lma, sections_data_len);

	log_debug("Zeroing bss sections:");
	zero_sections(sections_bss_vma, sections_bss_len);

	return 0;
}
