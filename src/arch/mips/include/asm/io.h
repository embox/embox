/**
 * @file
 *
 * @brief
 *
 * @date 27.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_IO_H_
#define MIPS_IO_H_

#include <types.h>

#define IO_OFFSET    0x1fd00000

#define out8(v,a) do { *((volatile uint8_t *)(a + IO_OFFSET)) = (uint8_t)(v); } while (0)
#define in8(a) (*(volatile uint8_t *)(a + IO_OFFSET))


#endif /* MIPS_IO_H_ */
