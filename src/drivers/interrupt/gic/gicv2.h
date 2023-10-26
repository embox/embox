/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 20.10.23
 */
#ifndef DRIVERS_INTERRUPT_GIC_GICV2_H_
#define DRIVERS_INTERRUPT_GIC_GICV2_H_

#include "gic_base.h"
#include "gicv1.h"

/* clang-format off */
#define GICC_AIAR     (GICC_BASE + 0x0020) /* Aliased Interrupt Acknowledge Register */
#define GICC_AEOIR    (GICC_BASE + 0x0024) /* Aliased End of Interrupt Register */
#define GICC_AHPPIR   (GICC_BASE + 0x0028) /* Aliased Highest Priority Pending Interrupt Register */
#define GICC_APR(n)   (GICC_BASE + 0x00d0 + 4 * n) /* Active Priorities Registers */
#define GICC_NSAPR(n) (GICC_BASE + 0x00e0 + 4 * n) /* Non-secure Active Priorities Registers */
#define GICC_DIR      (GICC_BASE + 0x1000) /* Deactivate Interrupt Register */
/* clang-format on */

#endif /* DRIVERS_INTERRUPT_GIC_GICV2_H_ */
