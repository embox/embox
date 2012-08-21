/**
 * @file
 *
 * @brief
 *
 * @date 06.04.2012
 * @author Anton Bondarev
 */

#ifndef ASM_CPU_H_
#define ASM_CPU_H_

#include <module/embox/arch/sparc/kernel/leon3.h>
#include <framework/mod/options.h>

#define CONFIG_NWINDOWS     \
	OPTION_MODULE_GET(embox__arch__sparc__kernel__leon3, NUMBER,nwindows)


#endif /* ASM_CPU_H_ */
