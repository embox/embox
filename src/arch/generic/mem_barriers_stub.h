/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 26.06.23
 */

#ifndef ARCH_GENERIC_MEM_BARRIERS_STUB_H_
#define ARCH_GENERIC_MEM_BARRIERS_STUB_H_

#ifndef __ASSEMBLER__
static inline void data_mem_barrier(void) {
}
#endif /* __ASSEMBLER__ */

#endif /* ARCH_GENERIC_MEM_BARRIERS_STUB_H_ */
