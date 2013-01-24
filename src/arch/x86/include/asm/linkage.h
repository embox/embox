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

#define C_LABEL(name) .globl name; name

#else

/* Force passing arguments on stack */
#define asmlinkage      __attribute__((regparm(0)))

/* Force passing arguments on registers */
#define fastcall        __attribute__((regparm(3)))

#endif

#endif /* LINKAGE_H_ */
