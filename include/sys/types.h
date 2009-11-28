/**
 * @file sys/types.h
 *
 * @date 24.11.2009
 * @author Anton Bondarev
 */

#ifndef SYS_TYPES_H_
#define SYS_TYPES_H_

#ifndef __ASSEMBLER__

typedef int size_t; /**<Used for sizes of objects*/
typedef unsigned long clock_t; /**<Used for system times in clock ticks or CLOCKS_PER_SEC*/
typedef unsigned long useconds_t; /**<Used for time in microseconds.*/

typedef unsigned long spinlock_t;
typedef __s32 socklen_t;
typedef __u32 gfp_t;

typedef struct {
	volatile int counter;
} atomic_t;

#endif /* __ASSEMBLER__ */

#endif /* SYS_TYPES_H_ */
