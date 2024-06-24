/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.06.24
 */

#ifndef COMPAT_LINUX_ASM_TYPES_H_
#define COMPAT_LINUX_ASM_TYPES_H_

#include <asm/bitsperlong.h>

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef int8_t __s8;
typedef uint8_t __u8;

typedef int16_t __s16;
typedef uint16_t __u16;

typedef int32_t __s32;
typedef uint32_t __u32;

typedef int64_t __s64;
typedef uint64_t __u64;

#endif /* __ASSEMBLER__ */

#endif /* COMPAT_LINUX_ASM_TYPES_H_ */
