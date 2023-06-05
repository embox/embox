/**
 * @file
 *
 * @date Jun 3, 2023
 * @author Anton Bondarev
 */

#include <framework/mod/types.h>
#include <framework/mod/self.h>

#include <framework/mod/mod_sect_info.h>

struct mod_sect_info *mod_get_sect_info(void *module, struct mod_sect_info *sect_info) {
	const struct mod *mod;

	mod = (void*)module;

	sect_info->bss_vma = mod_label(mod)->bss.vma;
	sect_info->bss_vma_end = mod_label(mod)->bss.vma + mod_label(mod)->bss.len;
	sect_info->data_vma = mod_label(mod)->data.vma;
	sect_info->data_vma_end = mod_label(mod)->data.vma + mod_label(mod)->data.len;

	return sect_info;
}
