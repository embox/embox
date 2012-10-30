/**
 * @file
 * @brief
 *
 * @date 30.10.2012
 * @author Anton Bulychev
 */

#ifndef LIB_ELFLOADER_H_
#define LIB_ELFLOADER_H_

#include <lib/libelf.h>
#include <lib/libdl.h>

typedef struct {
	dl_data linker_data;
} elfloader_t;

extern int elfloader_load(Elf32_Objlist *objlist);

#endif /* LIB_ELFLOADER_H_ */
