/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.06.23
 */
#ifndef KERNEL_KGDB_H_
#define KERNEL_KGDB_H_

#define KGDB 1

#ifndef __ASSEMBLER__

extern void kgdb_start(void *entry);

#endif /* !__ASSEMBLER__ */

#endif /* KERNEL_KGDB_H_ */
