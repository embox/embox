/**
 * @brief MIPS Virtual Address Space
 *
 * @date 24.04.23
 * @author Aleksey Zhmulin
 */

#ifndef MIPS_ADDRSPACE_H_
#define MIPS_ADDRSPACE_H_

#include <stdint.h>

#if (_MIPS_SZPTR == 32)

/*
 * 32-bit Address Space
 */
#define KUSEG 0x00000000 /* User Mapped */
#define KSEG0 0x80000000 /* Kernel Unmapped */
#define KSEG1 0xa0000000 /* Kernel Unmapped Uncached */
#define KSSEG 0xc0000000 /* Supervisor Mapped */
#define KSEG3 0xe0000000 /* Kernel Mapped */

#elif (_MIPS_SZPTR == 64)

/*
 * 32-bit Compatibility Address Space
 */
#define KUSEG 0x0000000000000000 /* User Mapped */
#define KSEG0 0xffffffff80000000 /* Kernel Unmapped */
#define KSEG1 0xffffffffa0000000 /* Kernel Unmapped Uncached */
#define KSSEG 0xffffffffc0000000 /* Supervisor Mapped */
#define KSEG3 0xffffffffe0000000 /* Kernel Mapped */

#endif /* _MIPS_SZPTR */

#define PHYSADDR(addr)  (0x1fffffff & (uintptr_t)(addr))
#define KSEG0ADDR(addr) (PHYSADDR(addr) | KSEG0)
#define KSEG1ADDR(addr) (PHYSADDR(addr) | KSEG1)
#define KSSEGADDR(addr) (PHYSADDR(addr) | KSSEG)
#define KSEG3ADDR(addr) (PHYSADDR(addr) | KSEG3)

#endif /* MIPS_ADDRSPACE_H_ */
