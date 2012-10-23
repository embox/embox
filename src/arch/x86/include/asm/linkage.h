/**
 * @file
 *
 * @date Aug 28, 2012
 * @author: Anton Bondarev
 */

#ifndef LINKAGE_H_
#define LINKAGE_H_

#ifdef __ASSEMBLER__

#define C_ENTRY(name) .globl name; .align 2; name

#endif

#endif /* LINKAGE_H_ */
