/**
 * @file
 *
 * @date Mar 23, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_ARCH_MIPS_SUBARCH_P5600_GLOBAL_CONTROL_BLOCK_GLOBAL_CONTROL_BLOCK_H_
#define SRC_ARCH_MIPS_SUBARCH_P5600_GLOBAL_CONTROL_BLOCK_GLOBAL_CONTROL_BLOCK_H_

#define GCR_CONFIG                   0x0000
#define GCR_BASE                     0x0008
#define GCR_BASE_UPPER               0x000C
#define GCR_CONTROL                  0x0010
#define GCR_CONTROL2                 0x0018
#define GCR_ACCESS                   0x0020
#define GCR_REV                      0x0030
#define GCR_ERROR_MASK               0x0040
#define GCR_ERROR_CAUSE              0x0048
#define GCR_ERROR_ADDR               0x0050
#define GCR_ERROR_ADDR_UPPER         0x0054
#define GCR_ERROR_MULT               0x0058
#define GCR_CUSTOM_BASE              0x0060
#define GCR_CUSTOM_BASE_UPPER        0x0064
#define GCR_CUSTOM_STATUS            0x0068
#define GCR_L2_ONLY_SYNC_BASE        0x0070
#define GCR_L2_ONLY_SYNC_BASE_UPPER  0x0074
#define GCR_GIC_BASE                 0x0080
# define GIC_EN                  (1 << 0)
#define GCR_GIC_BASE_UPPER           0x0084
#define GCR_CPC_BASE                 0x0088
#define GCR_CPC_BASE_UPPER           0x008C


extern uint32_t mips32_gcb_set_register(uint32_t offset, uint32_t val);

extern uint32_t mips32_gcb_get_register(uint32_t val);

#endif /* SRC_ARCH_MIPS_SUBARCH_P5600_GLOBAL_CONTROL_BLOCK_GLOBAL_CONTROL_BLOCK_H_ */
