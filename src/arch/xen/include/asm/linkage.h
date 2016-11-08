/**
 * @file
 * @brief Xen asm helpers
 *
 * @author  Anton Kozlov
 * @date    08.11.2016
 */

#ifndef XEN_LINKAGE_H_
#define XEN_LINKAGE_H_

#ifdef __ASSEMBLER__

#define C_ENTRY(name) .globl name; .align 4; name
#define C_LABEL(name) .globl name; name

#endif /* __ASSEMBLER__ */

#endif /* XEN_LINKAGE_H_ */

