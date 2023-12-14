/**
 * @file
 *
 * @date Nov 28, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_ARCH_ARM_IMX_IOMUXC_H_
#define SRC_ARCH_ARM_IMX_IOMUXC_H_

#include <stdint.h>

#define IOMUXC_GPR2                              0x008
#define IOMUXC_GPR3                              0x00C

#define IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_B          0x08C
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16         0x090
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17         0x094
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18         0x098
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA21         0x0A4
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA24         0x0B4
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA25         0x0B8
#define IOMUXC_SW_MUX_CTL_PAD_EIM_DATA28         0x0C4

#define IOMUXC_SW_MUX_CTL_PAD_EIM_LBA_B          0x108

#define IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA08       0x190

#define IOMUXC_SW_MUX_CTL_PAD_KEY_COL0           0x1F8
#define IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0           0x1FC

#define IOMUXC_SW_MUX_CTL_PAD_KEY_COL3           0x210
#define IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3           0x214

#define IOMUXC_SW_MUX_CTL_PAD_GPIO01             0x224

#define IOMUXC_SW_MUX_CTL_PAD_GPIO03             0x22C

#define IOMUXC_SW_MUX_CTL_PAD_GPIO06             0x230

#define IOMUXC_SW_MUX_CTL_PAD_GPIO05             0x23C

#define IOMUXC_SW_MUX_CTL_PAD_GPIO16             0x248

#define IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA08        0x278
#define IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA09        0x27C
#define IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10        0x280
#define IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11        0x284

#define IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7          0x2A8
#define IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6          0x2AC

#define IOMUXC_ECSPI2_CSPI_CLK_IN_SELECT_INPUT   0x810
#define IOMUXC_ECSPI2_MISO_SELECT_INPUT          0x814
#define IOMUXC_ECSPI2_MOSI_SELECT_INPUT          0x818
#define IOMUXC_ECSPI2_SS0_SELECT_INPUT           0x81C

#define IOMUXC_I2C1_SCL_IN_SELECT_INPUT          0x898
#define IOMUXC_I2C1_SDA_IN_SELECT_INPUT          0x89C
#define IOMUXC_I2C2_SCL_IN_SELECT_INPUT          0x8A0
#define IOMUXC_I2C2_SDA_IN_SELECT_INPUT          0x8A4
#define IOMUXC_I2C3_SCL_IN_SELECT_INPUT          0x8A8
#define IOMUXC_I2C3_SDA_IN_SELECT_INPUT          0x8AC

#define IOMUXC_UART1_UART_RX_DATA_SELECT_INPUT   0x920
#define IOMUXC_UART2_UART_RX_DATA_SELECT_INPUT   0x928
#define IOMUXC_UART3_UART_RX_DATA_SELECT_INPUT   0x930
#define IOMUXC_UART4_UART_RX_DATA_SELECT_INPUT   0x938

extern void iomuxc_write(uint32_t reg, uint32_t val);

extern uint32_t iomuxc_read(uint32_t reg);

#endif /* SRC_ARCH_ARM_IMX_IOMUXC_H_ */
