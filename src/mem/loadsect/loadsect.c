/**
 * @file
 * @brief
 *
 * @date   26.04.2021
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <util/array.h>
#include <util/log.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(load_all_sections);

/* Loads multiple sections. For example, all text sections. */
static void load_sections(void *vmas[], void *lmas[], unsigned int lens[], int n) {
	int i;

	for (i = 0; i < n; i++) {
		if (0 == lens[i]) {
			continue;
		}
		if (vmas[i] != lmas[i]) {
			log_debug("vma=%p, lma=%p, len=0x%x", vmas[i], lmas[i], lens[i]);
			memcpy(vmas[i], lmas[i], lens[i]);
		}
	}
}

/* Zeroes multiple sections. For example, all bss sections. */
static void zero_sections(void *vmas[], unsigned int lens[], int n) {
	int i;

	for (i = 0; i < n; i++) {
		if (0 == lens[i]) {
			continue;
		}
		log_debug("vma=%p, len=0x%x", vmas[i], lens[i]);
		memset(vmas[i], 0, lens[i]);
	}
}

static int load_all_sections(void) {
	log_debug("");
	log_debug("Loading text sections:");
	load_sections(sections_text_vma, sections_text_lma, sections_text_len,
	                 ARRAY_SIZE(sections_text_vma));
	log_debug("Loading rodata sections:");
	load_sections(sections_rodata_vma, sections_rodata_lma, sections_rodata_len,
	                 ARRAY_SIZE(sections_rodata_vma));
	log_debug("Loading data sections:");
	load_sections(sections_data_vma, sections_data_lma, sections_data_len,
	                 ARRAY_SIZE(sections_data_vma));
	log_debug("Zeroing bss sections:");
	zero_sections(sections_bss_vma, sections_bss_len,
	                 ARRAY_SIZE(sections_bss_vma));

	return 0;
}
