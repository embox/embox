/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.12.22
 */

#include <stdint.h>

#include "arch.h"

uint8_t break_instr[BREAK_INSTR_SZ] = {
#if defined(__arm__) && !defined(__thumb2__)
    0x70, 0x00, 0x20, 0xe1
#endif
};
