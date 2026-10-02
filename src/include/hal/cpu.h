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
#include <module/embox/arch/smp.h>

#include <module/embox/kernel/stack.h>

#define NCPU OPTION_MODULE_GET(embox__arch__cpu, NUMBER, cpu_count)

#define KERNEL_AP_STACK_SZ OPTION_MODULE_GET(embox__kernel__stack, NUMBER, ap_stack_size)
#define KERNEL_BSP_STACK_SZ OPTION_MODULE_GET(embox__kernel__stack, NUMBER, stack_size)

#ifndef NOSMP
#define SMP
#endif /* NOSMP */

#ifndef __ASSEMBLER__

#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * @brief Get current CPU id.
 */
extern unsigned int cpu_get_id(void);

__END_DECLS

#endif /* !__ASSEMBLER__ */

#endif /* HAL_CPU_H_ */
