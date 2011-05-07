/**
 * @file
 * @brief Necessary arch-dependent typedefs.
 *
 * @date 01.07.10
 * @author Anton Kozlov
 */

#ifndef ARM_TYPES_H_
#define ARM_TYPES_H_

#include <asm-generic/types32.h>

#ifndef __ASSEMBLER__

// TODO AT91_REG - global namespace pollution. -- Eldar
// TODO AT91_REG breaks code style conventions. -- Eldar
// FIXME AT91_REG - platform-specific definition in common header. -- Eldar
// XXX too many TODO's ^_^ -- Eldar
/** Hardware register definition for AT91. */
typedef volatile unsigned int AT91_REG;

#endif /* __ASSEMBLER__ */

#endif /* ARM_TYPES_H_ */
