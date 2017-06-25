/**
 * @file
 * @brief Defines specific features for working with
 * trap handlers in SPARC architecture.
 *
 * @date 15.03.10
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#ifndef SPARC_TRAPS_CORE_H_
#define SPARC_TRAPS_CORE_H_

#include <stdint.h>

/*FIXME move MMU_TABLE_SIZE to arch dependent config*/
#define TRAP_TABLE_SIZE      (256)

#define DIVZERO_FAULT        0x2A

#define MMU_DATA_SECUR_FAULT 0x09
#define MMU_INST_SECUR_FAULT 0x01
#define MMU_DATA_MISS_FAULT  MMU_DATA_SECUR_FAULT //XXX 0x2c please fill in
#define MMU_INST_MISS_FAULT  MMU_INST_SECUR_FAULT //XXX 0x3C please fill in

/** Defines handler for traps_dispatcher in microblaze archecture */
typedef int (*__trap_handler)(uint32_t nr, void *data);

#define CONFIG_MAX_HWTRAP_NUMBER    0x10
#define CONFIG_MAX_INTERRUPT_NUMBER 0x10
#define CONFIG_MAX_SOFTTRAP_NUMBER  0x60

/** Defines traps environment for sparc structure */
typedef struct __traps_env {
	uint32_t base_addr;
	__trap_handler hw_traps[CONFIG_MAX_HWTRAP_NUMBER];
	__trap_handler soft_traps[CONFIG_MAX_SOFTTRAP_NUMBER];
	__trap_handler interrupts[CONFIG_MAX_INTERRUPT_NUMBER];
}__traps_env_t;

#endif /* SPARC_TRAPS_CORE_H_ */
