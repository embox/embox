/**
 * @file
 * @brief Logical CPU ids and the MPIDR behind each one (aarch64).
 */
#ifndef AARCH64_CPU_ID_H_
#define AARCH64_CPU_ID_H_

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <sys/cdefs.h>

/** MPIDR_EL1 bits that identify a PE: Aff3 [39:32], Aff2.Aff1.Aff0 [23:0]. */
#define MPIDR_AFF_MASK 0xff00ffffffULL

__BEGIN_DECLS

/** This core's affinity fields, masked. */
extern uint64_t aarch64_mpidr(void);

/**
 * Give the calling core the logical id @a cpu_id: write TPIDR_EL1, which
 * cpu_get_id() reads, and record the core's MPIDR.
 */
extern void aarch64_cpu_id_register(unsigned int cpu_id);

/** MPIDR of a logical id, or 0 if that id has not registered. */
extern uint64_t aarch64_cpu_mpidr(unsigned int cpu_id);

__END_DECLS

#endif /* !__ASSEMBLER__ */

#endif /* AARCH64_CPU_ID_H_ */
