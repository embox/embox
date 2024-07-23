/**
 * @file
 *
 * @date Jun 3, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_FRAMEWORK_MOD_MOD_SECT_INFO_MOD_SECT_INFO_H_
#define SRC_FRAMEWORK_MOD_MOD_SECT_INFO_MOD_SECT_INFO_H_

#include <framework/mod/decls.h>
#include <framework/mod/types.h>

extern void *__MOD(__EMBUILD_MOD__);

struct mod_sect_info {
	void *data_vma;
	void *data_vma_end;
	void *bss_vma;
	void *bss_vma_end;
};

extern struct mod_sect_info *mod_get_sect_info(void *module,
    struct mod_sect_info *sect_info);

#define MOD_SELF_GET_SECT_INFO(sect_info) \
	mod_get_sect_info(&__MOD(__EMBUILD_MOD__), &sect_info);

#endif /* SRC_FRAMEWORK_MOD_MOD_SECT_INFO_MOD_SECT_INFO_H_ */
