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

extern int elfloader_place_relocatable(Elf32_Obj *obj);
extern int elfloader_place_shared(Elf32_Obj *obj);
extern int elfloader_place_executable(Elf32_Obj *obj);

#endif /* LIB_ELFLOADER_H_ */
