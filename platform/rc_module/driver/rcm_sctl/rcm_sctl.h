/**
 * @file
 *
 * @date Apr 11, 2023
 * @author Anton Bondarev
 */

#ifndef PLATFORM_RC_MODULE_DRIVER_RCM_SCTL_RCM_SCTL_H_
#define PLATFORM_RC_MODULE_DRIVER_RCM_SCTL_RCM_SCTL_H_

#include <stdint.h>

#define RCM_SCTL_SGMII_CTRL_STAT     0x014

#define RCM_SCTL_PCIE_RST            0x400
#define RCM_SCTL_PCIE_REG_0          0x404
#define RCM_SCTL_PCIE_REG_1          0x408
#define RCM_SCTL_PCIE_REG_2          0x40C


extern uint32_t rcm_sctl_read_reg(int reg);

extern void rcm_sctl_write_reg(int reg, uint32_t val);

#endif /* PLATFORM_RC_MODULE_DRIVER_RCM_SCTL_RCM_SCTL_H_ */
