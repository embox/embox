/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */

#include <types.h>
/*FIXME move MMU_TABLE_SIZE to arch dependent config*/
#define MMU_TABLE_SIZE 0x400

uint8_t test_mmu_table[MMU_TABLE_SIZE];

