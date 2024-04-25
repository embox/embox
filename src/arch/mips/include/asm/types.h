/**
 * @file
 * @brief Types for MIPS architecture.
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_TYPES_H_
#define MIPS_TYPES_H_

#if (_MIPS_SZPTR == 32)
#include <asm-generic/types32.h>
#elif (_MIPS_SZPTR == 64)
#include <asm-generic/types64.h>
#endif /* _MIPS_SZPTR */

#endif /* MIPS_TYPES_H_ */
