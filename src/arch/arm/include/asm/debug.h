/**
 * @file
 * @brief
 *
 * @date 22.08.23
 * @author Aleksey Zhmulin
 */
#ifndef ARM_DEBUG_H_
#define ARM_DEBUG_H_

#define DBGBCR_EN      (1U << 0)
#define DBGBCR_PMC_ANY (0b11U << 1)
#define DBGBCR_BAS_ANY (0b1111U << 5)

#define DBGDSCR_ITREN            (1U << 13)
#define DBGDSCR_MDBGEN           (1U << 15)
#define DBGDSCR_EXTDCCMODE_MASK  (3U << 20)
#define DBGDSCR_EXTDCCMODE_STALL (1U << 20)

#endif /* ARM_DEBUG_H_ */
