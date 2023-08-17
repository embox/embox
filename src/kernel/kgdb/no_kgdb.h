/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.06.23
 */
#ifndef KERNEL_NO_KGDB_H_
#define KERNEL_NO_KGDB_H_

#define KGDB 0

#ifndef __ASSEMBLER__

static inline void kgdb_start(void *entry) {
}

#endif /* !__ASSEMBLER__ */

#endif /* KERNEL_NO_KGDB_H_ */
