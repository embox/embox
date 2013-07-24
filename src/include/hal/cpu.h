/**
 * @file
 * @brief CPU definitions
 *
 * @date 24.07.13
 * @author Ilia Vaprol
 */

#ifndef HAL_CPU_H_
#define HAL_CPU_H_

#include <framework/mod/options.h>
#include <module/embox/arch/cpu.h>

/* FIXME hack to get abstract module variable */
#if OPTION_MODULE_DEFINED(embox__arch__cpu, NUMBER, cpu_count)
#define NCPU OPTION_MODULE_GET(embox__arch__cpu, NUMBER, cpu_count)
#elif OPTION_MODULE_DEFINED(embox__arch__generic__onecpu, NUMBER, cpu_count)
#define NCPU OPTION_MODULE_GET(embox__arch__generic__onecpu, NUMBER, cpu_count)
#elif OPTION_MODULE_DEFINED(embox__arch__x86__kernel__cpu, NUMBER, cpu_count)
#define NCPU OPTION_MODULE_GET(embox__arch__x86__kernel__cpu, NUMBER, cpu_count)
#endif

#ifndef __ASSEMBLER__

/**
 * Get CPU id
 */
extern unsigned int cpu_get_id(void);

#endif /* !__ASSEMBLER__ */

#endif /* HAL_CPU_H_ */
