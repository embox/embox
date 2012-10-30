/**
 * @file
 * @brief
 *
 * @date 16.10.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <stdlib.h>

#include <lib/libelf.h>

int elf_object_init(Elf32_Obj **obj, FILE *fd) {
	if (!(*obj = malloc(sizeof(Elf32_Obj)))) {
		return -ENOMEM;
	}

	**obj = (Elf32_Obj) {
		.fd             = fd,

		.need_reverse   = 0,
		.dyn_count      = 0,
		.rel_count      = 0,


		.header         = NULL,
		.sh_table       = NULL,
		.ph_table       = NULL,
		.string_table   = NULL,
		.sym_table      = NULL,
		.sym_names      = NULL,
		.dyn_section    = NULL,
		.rel_array      = NULL,
	};

	if (fd != NULL) {
		return elf_read_header(*obj);
	}

	return ENOERR;
}

void elf_object_free(Elf32_Obj *obj) {
	if (obj->header       != NULL) free(obj->header);
	if (obj->sh_table     != NULL) free(obj->sh_table);
	if (obj->ph_table     != NULL) free(obj->ph_table);
	if (obj->string_table != NULL) free(obj->string_table);
	if (obj->sym_table    != NULL) free(obj->sym_table);
	if (obj->sym_names    != NULL) free(obj->sym_names);
	if (obj->dyn_section  != NULL) free(obj->dyn_section);
	if (obj->rel_array    != NULL) free(obj->rel_array);

	free(obj);
}
