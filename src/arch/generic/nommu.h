/**
 * @file
 * @brief
 *
 * @date 12.11.12
 * @author Anton Bulychev
 */

#ifndef ARCH_GENERIC_NOMMU_H_
#define ARCH_GENERIC_NOMMU_H_

#define NOMMU
#define MMU_LEVELS 0

#ifndef __ASSEMBLER__
typedef int mmu_ctx_t;
#endif /* __ASSEMBLER__ */

#endif /* ARCH_GENERIC_NOMMU_H_ */
