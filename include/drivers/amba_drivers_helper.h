/**
 * \file amba_drivers_helper.h
 *
 * \date Jun 3, 2009
 * \author Anton Bondarev
 * \details must include only after definition dev_regs (pointer to struct registers). it contains macroses for hepling to write drivers module
 */

#ifndef AMBA_DRIVERS_HELPER_H_
#define AMBA_DRIVERS_HELPER_H_

#include "common.h"
#include "drivers/amba_pnp.h"

#ifdef _TEST_SYSTEM_
static AMBA_DEV amba_dev;
#endif

#define ASSERT_MODULE_INIT() assert(dev_regs != NULL)

#endif /* AMBA_DRIVERS_HELPER_H_ */
