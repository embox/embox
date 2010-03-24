/**
 * @file
 *
 * @brief Defines specific features for MMU in Microblaze architecture.
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_MMU_CORE_H_
#define MICROBLAZE_MMU_CORE_H_

/*FIXME move MMU_TABLE_SIZE to arch dependent config*/
#define MMU_TABLE_SIZE 0x400

/**
 * Describes structure for MMU environment for Microblaze architecture.
 */
typedef struct __mmu_env {
	uint32_t status;          /**< MMU enabled/disabled */

	uint32_t data_fault_cnt;  /**< Counter for data page faults */
	uint32_t inst_fault_cnt;  /**< Counter for instruction page faults */
	uint32_t fault_addr;      /**< Last fault address */
}__mmu_env_t;

#endif /* MICROBLAZE_MMU_CORE_H_ */
