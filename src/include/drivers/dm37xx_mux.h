/*
* (C) Copyright 2013
* Texas Instruments, <www.ti.com>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston,
* MA 02111-1307 USA
*/

#ifndef _MUX_H_
#define _MUX_H_

/*
 * M0 - Mux mode 0
 * M1 - Mux mode 1
 * M2 - Mux mode 2
 * M3 - Mux mode 3
 * M4 - Mux mode 4
 * M5 - Mux mode 5
 * M6 - Mux mode 6
 * M7 - Mux mode 7
 * IDIS - Input disabled
 * IEN - Input enabled
 * PD - Active-mode pull-down enabled
 * PU - Active-mode pull-up enabled
 * PI - Active-mode pull inhibited
 * SB_LOW - Standby mode configuration: Output low-level
 * SB_HI - Standby mode configuration: Output high-level
 * SB_HIZ - Standby mode configuration: Output hi-impedence
 * SB_PD - Standby mode pull-down enabled
 * SB_PU - Standby mode pull-up enabled
 * WKEN - Wakeup input enabled
 */

#define M0 0
#define M1 1
#define M2 2
#define M3 3
#define M4 4
#define M5 5
#define M6 6
#define M7 7
#define IDIS (0 << 8)
#define IEN (1 << 8)
#define PD (1 << 3)
#define PU (3 << 3)
#define PI (0 << 3)
#define SB_LOW (1 << 9)
#define SB_HI (5 << 9)
#define SB_HIZ (2 << 9)
#define SB_PD (1 << 12)
#define SB_PU (3 << 12)
#define WKEN (1 << 14)

/*
 * To get the physical address the offset has
 * to be added to OMAP37XX_CTRL_BASE
 */

#define OMAP37XX_CTRL_BASE						  0x48002000

#define CONTROL_PADCONF_GPIO_112                  0x0134
#define CONTROL_PADCONF_GPIO_113                  0x0136
#define CONTROL_PADCONF_GPIO_114                  0x0138
#define CONTROL_PADCONF_GPIO_115                  0x013A
#define CONTROL_PADCONF_GPIO_126                  0x0A54
#define CONTROL_PADCONF_GPIO_127                  0x0A56
#define CONTROL_PADCONF_GPIO_128                  0x0A58
#define CONTROL_PADCONF_GPIO_129                  0x0A5A
#define CONTROL_PADCONF_CAM_D0                    0x0116
#define CONTROL_PADCONF_CAM_D1                    0x0118
#define CONTROL_PADCONF_CAM_D2                    0x011A
#define CONTROL_PADCONF_CAM_D3                    0x011C
#define CONTROL_PADCONF_CAM_D4                    0x011E
#define CONTROL_PADCONF_CAM_D5                    0x0120
#define CONTROL_PADCONF_CAM_D6                    0x0122
#define CONTROL_PADCONF_CAM_D7                    0x0124
#define CONTROL_PADCONF_CAM_D8                    0x0126
#define CONTROL_PADCONF_CAM_D9                    0x0128
#define CONTROL_PADCONF_CAM_D10                   0x012A
#define CONTROL_PADCONF_CAM_D11                   0x012C
#define CONTROL_PADCONF_CAM_FLD                   0x0114
#define CONTROL_PADCONF_CAM_HS                    0x010C
#define CONTROL_PADCONF_CAM_PCLK                  0x0112
#define CONTROL_PADCONF_CAM_STROBE                0x0132
#define CONTROL_PADCONF_CAM_VS                    0x010E
#define CONTROL_PADCONF_CAM_WEN                   0x0130
#define CONTROL_PADCONF_CAM_XCLKA                 0x0110
#define CONTROL_PADCONF_CAM_XCLKB                 0x012E
#define CONTROL_PADCONF_DSS_ACBIAS                0x00DA
#define CONTROL_PADCONF_DSS_DATA0                 0x00DC
#define CONTROL_PADCONF_DSS_DATA1                 0x00DE
#define CONTROL_PADCONF_DSS_DATA2                 0x00E0
#define CONTROL_PADCONF_DSS_DATA3                 0x00E2
#define CONTROL_PADCONF_DSS_DATA4                 0x00E4
#define CONTROL_PADCONF_DSS_DATA5                 0x00E6
#define CONTROL_PADCONF_DSS_DATA6                 0x00E8
#define CONTROL_PADCONF_DSS_DATA7                 0x00EA
#define CONTROL_PADCONF_DSS_DATA8                 0x00EC
#define CONTROL_PADCONF_DSS_DATA9                 0x00EE
#define CONTROL_PADCONF_DSS_DATA10                0x00F0
#define CONTROL_PADCONF_DSS_DATA11                0x00F2
#define CONTROL_PADCONF_DSS_DATA12                0x00F4
#define CONTROL_PADCONF_DSS_DATA13                0x00F6
#define CONTROL_PADCONF_DSS_DATA14                0x00F8
#define CONTROL_PADCONF_DSS_DATA15                0x00FA
#define CONTROL_PADCONF_DSS_DATA16                0x00FC
#define CONTROL_PADCONF_DSS_DATA17                0x00FE
#define CONTROL_PADCONF_DSS_DATA18                0x0100
#define CONTROL_PADCONF_DSS_DATA19                0x0102
#define CONTROL_PADCONF_DSS_DATA20                0x0104
#define CONTROL_PADCONF_DSS_DATA21                0x0106
#define CONTROL_PADCONF_DSS_DATA22                0x0108
#define CONTROL_PADCONF_DSS_DATA23                0x010A
#define CONTROL_PADCONF_DSS_HSYNC                 0x00D6
#define CONTROL_PADCONF_DSS_PCLK                  0x00D4
#define CONTROL_PADCONF_DSS_VSYNC                 0x00D8
#define CONTROL_PADCONF_ETK_CLK_ES2               0x05D8
#define CONTROL_PADCONF_ETK_CTL_ES2               0x05DA
#define CONTROL_PADCONF_ETK_D0_ES2                0x05DC
#define CONTROL_PADCONF_ETK_D1_ES2                0x05DE
#define CONTROL_PADCONF_ETK_D2_ES2                0x05E0
#define CONTROL_PADCONF_ETK_D3_ES2                0x05E2
#define CONTROL_PADCONF_ETK_D4_ES2                0x05E4
#define CONTROL_PADCONF_ETK_D5_ES2                0x05E6
#define CONTROL_PADCONF_ETK_D6_ES2                0x05E8
#define CONTROL_PADCONF_ETK_D7_ES2                0x05EA
#define CONTROL_PADCONF_ETK_D8_ES2                0x05EC
#define CONTROL_PADCONF_ETK_D9_ES2                0x05EE
#define CONTROL_PADCONF_ETK_D10_ES2               0x05F0
#define CONTROL_PADCONF_ETK_D11_ES2               0x05F2
#define CONTROL_PADCONF_ETK_D12_ES2               0x05F4
#define CONTROL_PADCONF_ETK_D13_ES2               0x05F6
#define CONTROL_PADCONF_ETK_D14_ES2               0x05F8
#define CONTROL_PADCONF_ETK_D15_ES2               0x05FA
#define CONTROL_PADCONF_GPMC_A1                   0x007A
#define CONTROL_PADCONF_GPMC_A2                   0x007C
#define CONTROL_PADCONF_GPMC_A3                   0x007E
#define CONTROL_PADCONF_GPMC_A4                   0x0080
#define CONTROL_PADCONF_GPMC_A5                   0x0082
#define CONTROL_PADCONF_GPMC_A6                   0x0084
#define CONTROL_PADCONF_GPMC_A7                   0x0086
#define CONTROL_PADCONF_GPMC_A8                   0x0088
#define CONTROL_PADCONF_GPMC_A9                   0x008A
#define CONTROL_PADCONF_GPMC_A10                  0x008C
#define CONTROL_PADCONF_GPMC_A11                  0x0264
#define CONTROL_PADCONF_GPMC_CLK                  0x00BE
#define CONTROL_PADCONF_GPMC_D0                   0x008E
#define CONTROL_PADCONF_GPMC_D1                   0x0090
#define CONTROL_PADCONF_GPMC_D2                   0x0092
#define CONTROL_PADCONF_GPMC_D3                   0x0094
#define CONTROL_PADCONF_GPMC_D4                   0x0096
#define CONTROL_PADCONF_GPMC_D5                   0x0098
#define CONTROL_PADCONF_GPMC_D6                   0x009A
#define CONTROL_PADCONF_GPMC_D7                   0x009C
#define CONTROL_PADCONF_GPMC_D8                   0x009E
#define CONTROL_PADCONF_GPMC_D9                   0x00A0
#define CONTROL_PADCONF_GPMC_D10                  0x00A2
#define CONTROL_PADCONF_GPMC_D11                  0x00A4
#define CONTROL_PADCONF_GPMC_D12                  0x00A6
#define CONTROL_PADCONF_GPMC_D13                  0x00A8
#define CONTROL_PADCONF_GPMC_D14                  0x00AA
#define CONTROL_PADCONF_GPMC_D15                  0x00AC
#define CONTROL_PADCONF_GPMC_NADV_ALE             0x00C0
#define CONTROL_PADCONF_GPMC_NBE0_CLE             0x00C6
#define CONTROL_PADCONF_GPMC_NBE1                 0x00C8
#define CONTROL_PADCONF_GPMC_NCS0                 0x00AE
#define CONTROL_PADCONF_GPMC_NCS1                 0x00B0
#define CONTROL_PADCONF_GPMC_NCS2                 0x00B2
#define CONTROL_PADCONF_GPMC_NCS3                 0x00B4
#define CONTROL_PADCONF_GPMC_NCS4                 0x00B6
#define CONTROL_PADCONF_GPMC_NCS5                 0x00B8
#define CONTROL_PADCONF_GPMC_NCS6                 0x00BA
#define CONTROL_PADCONF_GPMC_NCS7                 0x00BC
#define CONTROL_PADCONF_GPMC_NOE                  0x00C2
#define CONTROL_PADCONF_GPMC_NWE                  0x00C4
#define CONTROL_PADCONF_GPMC_NWP                  0x00CA
#define CONTROL_PADCONF_GPMC_WAIT0                0x00CC
#define CONTROL_PADCONF_GPMC_WAIT1                0x00CE
#define CONTROL_PADCONF_GPMC_WAIT2                0x00D0
#define CONTROL_PADCONF_GPMC_WAIT3                0x00D2
#define CONTROL_PADCONF_HDQ_SIO                   0x01C6
#define CONTROL_PADCONF_HSUSB0_CLK                0x01A2
#define CONTROL_PADCONF_HSUSB0_DATA0              0x01AA
#define CONTROL_PADCONF_HSUSB0_DATA1              0x01AC
#define CONTROL_PADCONF_HSUSB0_DATA2              0x01AE
#define CONTROL_PADCONF_HSUSB0_DATA3              0x01B0
#define CONTROL_PADCONF_HSUSB0_DATA4              0x01B2
#define CONTROL_PADCONF_HSUSB0_DATA5              0x01B4
#define CONTROL_PADCONF_HSUSB0_DATA6              0x01B6
#define CONTROL_PADCONF_HSUSB0_DATA7              0x01B8
#define CONTROL_PADCONF_HSUSB0_DIR                0x01A6
#define CONTROL_PADCONF_HSUSB0_NXT                0x01A8
#define CONTROL_PADCONF_HSUSB0_STP                0x01A4
#define CONTROL_PADCONF_I2C1_SCL                  0x01BA
#define CONTROL_PADCONF_I2C1_SDA                  0x01BC
#define CONTROL_PADCONF_I2C2_SCL                  0x01BE
#define CONTROL_PADCONF_I2C2_SDA                  0x01C0
#define CONTROL_PADCONF_I2C3_SCL                  0x01C2
#define CONTROL_PADCONF_I2C3_SDA                  0x01C4
#define CONTROL_PADCONF_I2C4_SCL                  0x0A00
#define CONTROL_PADCONF_I2C4_SDA                  0x0A02
#define CONTROL_PADCONF_JTAG_EMU0                 0x0A24
#define CONTROL_PADCONF_JTAG_EMU1                 0x0A26
#define CONTROL_PADCONF_JTAG_NTRST                0x0A1C
#define CONTROL_PADCONF_JTAG_RTCK                 0x0A4E
#define CONTROL_PADCONF_JTAG_TCK                  0x0A1E
#define CONTROL_PADCONF_JTAG_TDI                  0x0A22
#define CONTROL_PADCONF_JTAG_TDO                  0x0A50
#define CONTROL_PADCONF_JTAG_TMS                  0x0A20
#define CONTROL_PADCONF_MCBSP_CLKS                0x0194
#define CONTROL_PADCONF_MCBSP1_CLKR               0x018C
#define CONTROL_PADCONF_MCBSP1_CLKX               0x0198
#define CONTROL_PADCONF_MCBSP1_DR                 0x0192
#define CONTROL_PADCONF_MCBSP1_DX                 0x0190
#define CONTROL_PADCONF_MCBSP1_FSR                0x018E
#define CONTROL_PADCONF_MCBSP1_FSX                0x0196
#define CONTROL_PADCONF_MCBSP2_CLKX               0x013E
#define CONTROL_PADCONF_MCBSP2_DR                 0x0140
#define CONTROL_PADCONF_MCBSP2_DX                 0x0142
#define CONTROL_PADCONF_MCBSP2_FSX                0x013C
#define CONTROL_PADCONF_MCBSP3_CLKX               0x0170
#define CONTROL_PADCONF_MCBSP3_DR                 0x016E
#define CONTROL_PADCONF_MCBSP3_DX                 0x016C
#define CONTROL_PADCONF_MCBSP3_FSX                0x0172
#define CONTROL_PADCONF_MCBSP4_CLKX               0x0184
#define CONTROL_PADCONF_MCBSP4_DR                 0x0186
#define CONTROL_PADCONF_MCBSP4_DX                 0x0188
#define CONTROL_PADCONF_MCBSP4_FSX                0x018A
#define CONTROL_PADCONF_MCSPI1_CLK                0x01C8
#define CONTROL_PADCONF_MCSPI1_CS0                0x01CE
#define CONTROL_PADCONF_MCSPI1_CS1                0x01D0
#define CONTROL_PADCONF_MCSPI1_CS2                0x01D2
#define CONTROL_PADCONF_MCSPI1_CS3                0x01D4
#define CONTROL_PADCONF_MCSPI1_SIMO               0x01CA
#define CONTROL_PADCONF_MCSPI1_SOMI               0x01CC
#define CONTROL_PADCONF_MCSPI2_CLK                0x01D6
#define CONTROL_PADCONF_MCSPI2_CS0                0x01DC
#define CONTROL_PADCONF_MCSPI2_CS1                0x01DE
#define CONTROL_PADCONF_MCSPI2_SIMO               0x01D8
#define CONTROL_PADCONF_MCSPI2_SOMI               0x01DA
#define CONTROL_PADCONF_MMC1_CLK                  0x0144
#define CONTROL_PADCONF_MMC1_CMD                  0x0146
#define CONTROL_PADCONF_MMC1_DAT0                 0x0148
#define CONTROL_PADCONF_MMC1_DAT1                 0x014A
#define CONTROL_PADCONF_MMC1_DAT2                 0x014C
#define CONTROL_PADCONF_MMC1_DAT3                 0x014E
#define CONTROL_PADCONF_MMC2_CLK                  0x0158
#define CONTROL_PADCONF_MMC2_CMD                  0x015A
#define CONTROL_PADCONF_MMC2_DAT0                 0x015C
#define CONTROL_PADCONF_MMC2_DAT1                 0x015E
#define CONTROL_PADCONF_MMC2_DAT2                 0x0160
#define CONTROL_PADCONF_MMC2_DAT3                 0x0162
#define CONTROL_PADCONF_MMC2_DAT4                 0x0164
#define CONTROL_PADCONF_MMC2_DAT5                 0x0166
#define CONTROL_PADCONF_MMC2_DAT6                 0x0168
#define CONTROL_PADCONF_MMC2_DAT7                 0x016A
#define CONTROL_PADCONF_SDRC_A0                   0x05A4
#define CONTROL_PADCONF_SDRC_A1                   0x05A6
#define CONTROL_PADCONF_SDRC_A2                   0x05A8
#define CONTROL_PADCONF_SDRC_A3                   0x05AA
#define CONTROL_PADCONF_SDRC_A4                   0x05AC
#define CONTROL_PADCONF_SDRC_A5                   0x05AE
#define CONTROL_PADCONF_SDRC_A6                   0x05B0
#define CONTROL_PADCONF_SDRC_A7                   0x05B2
#define CONTROL_PADCONF_SDRC_A8                   0x05B4
#define CONTROL_PADCONF_SDRC_A9                   0x05B6
#define CONTROL_PADCONF_SDRC_A10                  0x05B8
#define CONTROL_PADCONF_SDRC_A11                  0x05BA
#define CONTROL_PADCONF_SDRC_A12                  0x05BC
#define CONTROL_PADCONF_SDRC_A13                  0x05BE
#define CONTROL_PADCONF_SDRC_A14                  0x05C0
#define CONTROL_PADCONF_SDRC_BA0                  0x05A0
#define CONTROL_PADCONF_SDRC_BA1                  0x05A2
#define CONTROL_PADCONF_SDRC_CKE0                 0x0262
#define CONTROL_PADCONF_SDRC_CKE1                 0x0264
#define CONTROL_PADCONF_SDRC_CLK                  0x0070
#define CONTROL_PADCONF_SDRC_D0                   0x0030
#define CONTROL_PADCONF_SDRC_D1                   0x0032
#define CONTROL_PADCONF_SDRC_D2                   0x0034
#define CONTROL_PADCONF_SDRC_D3                   0x0036
#define CONTROL_PADCONF_SDRC_D4                   0x0038
#define CONTROL_PADCONF_SDRC_D5                   0x003A
#define CONTROL_PADCONF_SDRC_D6                   0x003C
#define CONTROL_PADCONF_SDRC_D7                   0x003E
#define CONTROL_PADCONF_SDRC_D8                   0x0040
#define CONTROL_PADCONF_SDRC_D9                   0x0042
#define CONTROL_PADCONF_SDRC_D10                  0x0044
#define CONTROL_PADCONF_SDRC_D11                  0x0046
#define CONTROL_PADCONF_SDRC_D12                  0x0048
#define CONTROL_PADCONF_SDRC_D13                  0x004A
#define CONTROL_PADCONF_SDRC_D14                  0x004C
#define CONTROL_PADCONF_SDRC_D15                  0x004E
#define CONTROL_PADCONF_SDRC_D16                  0x0050
#define CONTROL_PADCONF_SDRC_D17                  0x0052
#define CONTROL_PADCONF_SDRC_D18                  0x0054
#define CONTROL_PADCONF_SDRC_D19                  0x0056
#define CONTROL_PADCONF_SDRC_D20                  0x0058
#define CONTROL_PADCONF_SDRC_D21                  0x005A
#define CONTROL_PADCONF_SDRC_D22                  0x005C
#define CONTROL_PADCONF_SDRC_D23                  0x005E
#define CONTROL_PADCONF_SDRC_D24                  0x0060
#define CONTROL_PADCONF_SDRC_D25                  0x0062
#define CONTROL_PADCONF_SDRC_D26                  0x0064
#define CONTROL_PADCONF_SDRC_D27                  0x0066
#define CONTROL_PADCONF_SDRC_D28                  0x0068
#define CONTROL_PADCONF_SDRC_D29                  0x006A
#define CONTROL_PADCONF_SDRC_D30                  0x006C
#define CONTROL_PADCONF_SDRC_D31                  0x006E
#define CONTROL_PADCONF_SDRC_DM0                  0x05CE
#define CONTROL_PADCONF_SDRC_DM1                  0x05D0
#define CONTROL_PADCONF_SDRC_DM2                  0x05D2
#define CONTROL_PADCONF_SDRC_DM3                  0x05D4
#define CONTROL_PADCONF_SDRC_DQS0                 0x0072
#define CONTROL_PADCONF_SDRC_DQS1                 0x0074
#define CONTROL_PADCONF_SDRC_DQS2                 0x0076
#define CONTROL_PADCONF_SDRC_DQS3                 0x0078
#define CONTROL_PADCONF_SDRC_NCAS                 0x05CA
#define CONTROL_PADCONF_SDRC_NCLK                 0x05C6
#define CONTROL_PADCONF_SDRC_NCS0                 0x05C2
#define CONTROL_PADCONF_SDRC_NCS1                 0x05C4
#define CONTROL_PADCONF_SDRC_NRAS                 0x05C8
#define CONTROL_PADCONF_SDRC_NWE                  0x05CC
#define CONTROL_PADCONF_SYS_32K                   0x0A04
#define CONTROL_PADCONF_SYS_BOOT0                 0x0A0A
#define CONTROL_PADCONF_SYS_BOOT1                 0x0A0C
#define CONTROL_PADCONF_SYS_BOOT2                 0x0A0E
#define CONTROL_PADCONF_SYS_BOOT3                 0x0A10
#define CONTROL_PADCONF_SYS_BOOT4                 0x0A12
#define CONTROL_PADCONF_SYS_BOOT5                 0x0A14
#define CONTROL_PADCONF_SYS_BOOT6                 0x0A16
#define CONTROL_PADCONF_SYS_CLKOUT1               0x0A1A
#define CONTROL_PADCONF_SYS_CLKOUT2               0x01E2
#define CONTROL_PADCONF_SYS_CLKREQ                0x0A06
#define CONTROL_PADCONF_SYS_NIRQ                  0x01E0
#define CONTROL_PADCONF_SYS_NRESWARM              0x0A08
#define CONTROL_PADCONF_SYS_OFF_MODE              0x0A18
#define CONTROL_PADCONF_UART1_CTS                 0x0180
#define CONTROL_PADCONF_UART1_RTS                 0x017E
#define CONTROL_PADCONF_UART1_RX                  0x0182
#define CONTROL_PADCONF_UART1_TX                  0x017C
#define CONTROL_PADCONF_UART2_CTS                 0x0174
#define CONTROL_PADCONF_UART2_RTS                 0x0176
#define CONTROL_PADCONF_UART2_RX                  0x017A
#define CONTROL_PADCONF_UART2_TX                  0x0178
#define CONTROL_PADCONF_UART3_CTS_RCTX            0x019A
#define CONTROL_PADCONF_UART3_RTS_SD              0x019C
#define CONTROL_PADCONF_UART3_RX_IRRX             0x019E
#define CONTROL_PADCONF_UART3_TX_IRTX             0x01A0

#define __arch_putw(v,a)		(*(volatile unsigned short *)(a) = (v))
#define writew(v,a)				__arch_putw(v,a)

#define MUX_VAL(OFFSET,VALUE)\
     writew((VALUE), OMAP37XX_CTRL_BASE + (OFFSET));

#endif
