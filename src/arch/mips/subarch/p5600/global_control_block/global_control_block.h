/**
 * @file
 *
 * @date Mar 23, 2020
 * @author Anton Bondarev
 */

#ifndef MIPS_SUBARCH_P5600_GLOBAL_CONTROL_BLOCK_H_
#define MIPS_SUBARCH_P5600_GLOBAL_CONTROL_BLOCK_H_

#define GCR_CONFIG                  0x0000
#define GCR_BASE                    0x0008
#define GCR_BASE_UPPER              0x000C
#define GCR_CONTROL                 0x0010
#define GCR_CONTROL2                0x0018
#define GCR_ACCESS                  0x0020
#define GCR_REV                     0x0030
#define GCR_ERROR_MASK              0x0040
#define GCR_ERROR_CAUSE             0x0048
#define GCR_ERROR_ADDR              0x0050
#define GCR_ERROR_ADDR_UPPER        0x0054
#define GCR_ERROR_MULT              0x0058
#define GCR_CUSTOM_BASE             0x0060
#define GCR_CUSTOM_BASE_UPPER       0x0064
#define GCR_CUSTOM_STATUS           0x0068
#define GCR_L2_ONLY_SYNC_BASE       0x0070
#define GCR_L2_ONLY_SYNC_BASE_UPPER 0x0074
#define GCR_GIC_BASE                0x0080
#define GIC_EN                      (1 << 0)
#define GCR_GIC_BASE_UPPER          0x0084
#define GCR_CPC_BASE                0x0088
#define GCR_CPC_BASE_UPPER          0x008C
#define GCR_L2_CONFIG               0x0130

/* GCR_L2_CONFIG fields */
#define GCR_L2_CONFIG_ASSOC_SHIFT   0
#define GCR_L2_CONFIG_ASSOC_BITS    8
#define GCR_L2_CONFIG_LINESZ_SHIFT  8
#define GCR_L2_CONFIG_LINESZ_BITS   4
#define GCR_L2_CONFIG_SETSZ_SHIFT   12
#define GCR_L2_CONFIG_SETSZ_BITS    4
#define GCR_L2_CONFIG_BYPASS        (1 << 20)

extern uint32_t mips32_gcb_set_register(uint32_t offset, uint32_t val);

extern uint32_t mips32_gcb_get_register(uint32_t val);

extern uint32_t mips_cm_l2_line_size(void);

#endif /* MIPS_SUBARCH_P5600_GLOBAL_CONTROL_BLOCK_H_ */
