/**
 * @file
 * @brief PSCI calls used to bring secondary cores up (aarch64).
 */
#ifndef AARCH64_PSCI_H_
#define AARCH64_PSCI_H_

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

/* PSCI return codes */
#define PSCI_SUCCESS           0
#define PSCI_NOT_SUPPORTED     (-1)
#define PSCI_INVALID_PARAMS    (-2)
#define PSCI_DENIED            (-3)
#define PSCI_ALREADY_ON        (-4)

__BEGIN_DECLS

/** False when this board has no PSCI conduit configured. */
extern bool psci_available(void);

/** "smc", "hvc" or "(none)", for logs. */
extern const char *psci_method(void);

extern long psci_version(void);

/**
 * Power the core @a mpidr on. It starts at the physical address @a entry with
 * the MMU and caches off, at the highest non-secure EL, x0 = @a context_id.
 */
extern long psci_cpu_on(uint64_t mpidr, uintptr_t entry, uint64_t context_id);

__END_DECLS

#endif /* !__ASSEMBLER__ */

#endif /* AARCH64_PSCI_H_ */
