/**
 * @file
 * @brief Implementation of binary align, both for C and assembly.
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_BINALIGN_H_
# error "Do not include this file directly, use <util/binalign.h> instead!"
#endif /* UTIL_BINALIGN_H_ */

#define __binalign_mask(x, mask)       (((x) + (mask)) & ~(mask))

#define __binalign(x, power) \
		__binalign_bound(x, 0x1 << (power))

#ifndef __ASSEMBLER__

# define __binalign_bound(x, bound) \
		__binalign_mask(x, (typeof(x)) (bound) - 1)

#else /* __ASSEMBLER__ */

# define __binalign_bound(x, bound) \
		__binalign_mask(x, (bound) - 1)

#endif /* __ASSEMBLER__ */

#define __binalign_check_mask(x, mask)  ((x) & (mask) == 0)

#ifndef __ASSEMBLER__

# define __binalign_check(x, power) \
		__binalign_check_bound(x, 0x1 << (power))

# define __binalign_check_bound(x, bound) \
		__binalign_check_mask(x, (typeof(x)) (bound) - 1)

#endif /* __ASSEMBLER__ */

