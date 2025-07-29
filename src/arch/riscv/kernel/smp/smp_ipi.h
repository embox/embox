/**
 * @file
 * @brief
 *
 * @date 30.08.2024
 * @author Zeng Zixian
 */

#ifndef RISCV_KERNEL_SMP_IPI_H_
#define RISCV_KERNEL_SMP_IPI_H_

#ifndef __ASSEMBLER__

enum ipi_type {
	NONE,    /* no additional information */
	RESCHED, /* ask target cpu to schedule */
};

extern enum ipi_type ipi_message;

extern void smp_send_resched(int cpu_id);
extern void smp_send_none(int cpu_id);

extern void smp_ack_ipi(void);
extern enum ipi_type smp_get_ipi_message(void);

extern void resched(void);

#endif

#endif /* RISCV_KERNEL_SMP_IPI_H_ */
