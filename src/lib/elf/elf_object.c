/**
 * @file
 * @brief
 *
 * @date 16.10.2012
 * @author Anton Bulychev
 */

#include <lib/libelf.h>
#include <stdlib.h>

void elf_initialize_object(Elf32_Obj *obj) {
	obj->need_reverse = 0;
	obj->dyn_count    = 0;
	obj->rel_count    = 0;

	obj->header       = NULL;
	obj->sh_table     = NULL;
	obj->ph_table     = NULL;
	obj->string_table = NULL;
	obj->sym_table    = NULL;
	obj->sym_names    = NULL;
	obj->dyn_section  = NULL;
	obj->rel_array    = NULL;
}

void elf_finilize_object(Elf32_Obj *obj) {
	obj->need_reverse = 0;
	obj->dyn_count = 0;
	obj->rel_count = 0;

	if (obj->header       != NULL) free(obj->header);
	if (obj->sh_table     != NULL) free(obj->sh_table);
	if (obj->ph_table     != NULL) free(obj->ph_table);
	if (obj->string_table != NULL) free(obj->string_table);
	if (obj->sym_table    != NULL) free(obj->sym_table);
	if (obj->sym_names    != NULL) free(obj->sym_names);
	if (obj->dyn_section  != NULL) free(obj->dyn_section);
	if (obj->rel_array    != NULL) free(obj->rel_array);
}
