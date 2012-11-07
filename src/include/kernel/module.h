/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_MODULE_H_
#define KERNEL_MODULE_H_

#include <lib/libelf.h>

extern Elf32_Addr find_symbol_addr(const char *name);

#endif /* KERNEL_MODULE_H_ */
