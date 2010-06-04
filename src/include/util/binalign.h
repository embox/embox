/**
 * @file
 * @brief TODO
 *
 * @date 04.06.2010
 * @author Eldar Abusalimov
 */

#ifndef BINALIGN_H_
#define BINALIGN_H_

#define binalign(x, shift) \
		binalign_bound(x, 0x1 << (shift))

#ifndef __ASSEMBLER__

#define binalign_bound(x, bound) \
		__binalign_mask(x, (typeof(x)) (bound) - 1)

#else /* __ASSEMBLER__ */

#define binalign_bound(x, bound) \
		__binalign_mask(x, (bound) - 1)

#endif /* __ASSEMBLER__ */

#define __binalign_mask(x, mask)       (((x) + (mask)) & ~(mask))

#ifndef __ASSEMBLER__

#define binalign_check(x, shift) \
		binalign_check_bound(x, 0x1 << (shift))

#define binalign_check_bound(x, bound) \
		__binalign_check_mask(x, (typeof(x)) (bound) - 1)

#define __binalign_check_mask(x, mask)  ((x) & (mask) == 0)

#endif /* __ASSEMBLER__ */

#endif /* BINALIGN_H_ */
