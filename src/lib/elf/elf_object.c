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
	int err;

	if (!(*obj = malloc(sizeof(Elf32_Obj)))) {
		return -ENOMEM;
	}

	**obj = (Elf32_Obj) {
		.fd             = fd,

		.need_reverse   = 0,
		.dyn_count      = 0,
		.rel_count      = 0,


		.header         = NULL,
		.sections       = NULL,
		.sh_table       = NULL,
		.ph_table       = NULL,
		.string_table   = NULL,
		.sym_table      = NULL,
		.sym_names      = NULL,
		.dyn_section    = NULL,
		.rel_array      = NULL,
	};

	if (fd != NULL) {
		/* Read elf header and allocate array for sections */

		if ((err = elf_read_header(*obj)) < 0) {
			return err;
		}

		return elf_init_sections(*obj);
	}

	return ENOERR;
}

void elf_object_free(Elf32_Obj *obj) {
	if (obj->sections != NULL) elf_free_sections(obj);
	if (obj->header   != NULL) free(obj->header);
	free(obj);
}
