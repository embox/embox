/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 20.10.23
 */
#ifndef DRIVERS_INTERRUPT_GIC_GIC_UTIL_H_
#define DRIVERS_INTERRUPT_GIC_GIC_UTIL_H_

#include <framework/mod/options.h>

#define GICD_BASE OPTION_GET(NUMBER, gicd_base)
#define GICR_BASE OPTION_GET(NUMBER, gicr_base)
#define GICC_BASE OPTION_GET(NUMBER, gicc_base)
#define GICV_BASE OPTION_GET(NUMBER, gicv_base)
#define GICH_BASE OPTION_GET(NUMBER, gich_base)

#endif /* DRIVERS_INTERRUPT_GIC_GIC_UTIL_H_ */
