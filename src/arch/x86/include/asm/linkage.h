/**
 * @file
 *
 * @date Aug 28, 2012
 * @author: Anton Bondarev
 */

#include <asm/gdt.h>
#include <framework/mod/options.h>
#include <module/embox/arch/x86/kernel/smp.h>

#ifndef LINKAGE_H_
#define LINKAGE_H_

#ifdef __ASSEMBLER__

#define C_ENTRY(name) .globl name; .align 2; name

#define C_LABEL(name) .globl name; name

#define MAX_SUPPORTED_CORES OPTION_MODULE_GET(embox__arch__x86__kernel__smp, NUMBER, max_supported_cores)

#else

/* Force passing arguments on stack */
#define asmlinkage      __attribute__((regparm(0)))

/* Force passing arguments on registers */
#define fastcall        __attribute__((regparm(3)))

#endif

#endif /* LINKAGE_H_ */
