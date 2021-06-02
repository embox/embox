/**
 * @file
 *
 * @date Jun 2, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_LINUX_INCLUDE_ASM_ADDRSPACE_H_
#define SRC_COMPAT_LINUX_INCLUDE_ASM_ADDRSPACE_H_


/*
 * Memory segments (32bit kernel mode addresses)
 * These are the traditional names used in the 32-bit universe.
 */
#define KUSEG       (0x00000000)
#define KSEG0       (0x80000000)
#define KSEG1       (0xa0000000)


#endif /* SRC_COMPAT_LINUX_INCLUDE_ASM_ADDRSPACE_H_ */
