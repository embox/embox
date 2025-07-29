/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.07.25
 */

#ifndef RISCV_KERNEL_CLINT_H_
#define RISCV_KERNEL_CLINT_H_

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

extern void clint_send_ipi(unsigned int hart_id);
extern void clint_clear_ipi(void);

extern uint64_t clint_get_time(void);
extern void clint_set_timer(uint64_t val);
extern void clint_set_timer_cpu(unsigned int hart_id, uint64_t val);

__END_DECLS

#endif /* RISCV_KERNEL_CLINT_H_ */
