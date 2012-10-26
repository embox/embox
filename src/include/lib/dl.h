/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#ifndef LIB_DL_H_
#define LIB_DL_H_

extern int elf_relocate(FILE *fd, Elf32_Obj *obj, uint32_t *offsets);

#endif /* LIB_DL_H_ */
