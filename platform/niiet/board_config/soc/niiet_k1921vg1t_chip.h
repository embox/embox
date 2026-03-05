#ifndef BOARD_CONFIG_NIIET_K1921VG015_CHIP_H_
#define BOARD_CONFIG_NIIET_K1921VG015_CHIP_H_


/******************************************************************************/
/*                          Peripheral memory map                             */
/******************************************************************************/
#define GPIOA_BASE             (0x40000000UL)
#define GPIOB_BASE             (0x40001000UL)
#define GPIOC_BASE             (0x40002000UL)
#define GPIOD_BASE             (0x40003000UL)
#define GPIOE_BASE             (0x40004000UL)
#define GPIOF_BASE             (0x40005000UL)
#define GPIOG_BASE             (0x40006000UL)
#define CANFD0_BASE            (0x41000000UL)
#define CANFD1_BASE            (0x41001000UL)
#define CANFD2_BASE            (0x41002000UL)
#define CANFD3_BASE            (0x41003000UL)
#define CAN_BASE               (0x41010000UL)
#define CANMSG_BASE            (0x41011000UL)
#define ADC_BASE               (0x41020000UL)
#define QSPI0_BASE             (0x41030000UL)
#define QSPI1_BASE             (0x41040000UL)
#define CRC_BASE               (0x41050000UL)
#define HASH_BASE              (0x41060000UL)
#define CRYPTO_BASE            (0x41070000UL)
#define USBDC0_BASE            (0x41080000UL)
#define USBDC1_BASE            (0x41090000UL)
#define USBCTR0_BASE           (0x41080800UL)
#define USBCTR1_BASE           (0x41090800UL)
#define USBHC0_BASE            (0x41081000UL)
#define USBHC1_BASE            (0x41091000UL)
#define DMA_BASE               (0x50000000UL)
#define FLASHM_BASE            (0x50002000UL)
#define SIU_BASE               (0x50003000UL)
#define RCU_BASE               (0x50004000UL)
#define RTC_BASE               (0x50005000UL)
#define WDT_BASE               (0x50006000UL)
#define SRAM_BASE              (0x50007000UL)
#define TRNG_BASE              (0x50008000UL)
#define EMC_BASE               (0x50009000UL)
#define SDRAM_BASE             (0x5000A000UL)
#define ETH_BASE               (0x5000B000UL)
#define FLASHD_BASE            (0x5000C000UL)
#define PWM0_BASE              (0x5000D000UL)
#define PWM1_BASE              (0x5000E000UL)
#define PWM2_BASE              (0x5000F000UL)
#define PWM3_BASE              (0x50010000UL)
#define PWM4_BASE              (0x50011000UL)
#define PWM5_BASE              (0x50012000UL)
#define PWM6_BASE              (0x50013000UL)
#define PWM7_BASE              (0x50014000UL)
#define PWM8_BASE              (0x50015000UL)
#define PWM9_BASE              (0x50016000UL)
#define PWM10_BASE             (0x50017000UL)
#define PWM11_BASE             (0x50018000UL)
#define PWM12_BASE             (0x50019000UL)
#define PWM13_BASE             (0x5001A000UL)
#define PWM14_BASE             (0x5001B000UL)
#define PWM15_BASE             (0x5001C000UL)
#define ACMP0_BASE             (0x51000000UL)
#define ACMP1_BASE             (0x51001000UL)
#define ACMP2_BASE             (0x51002000UL)
#define ACMP3_BASE             (0x51003000UL)
#define DAC0_BASE              (0x51004000UL)
#define DAC1_BASE              (0x51005000UL)
#define I2S0_BASE              (0x51006000UL)
#define I2S1_BASE              (0x51007000UL)
#define TMR0_BASE              (0x51008000UL)
#define TMR1_BASE              (0x51009000UL)
#define TMR2_BASE              (0x5100A000UL)
#define TMR3_BASE              (0x5100B000UL)
#define TMR4_BASE              (0x5100C000UL)
#define TMR5_BASE              (0x5100D000UL)
#define TMR6_BASE              (0x5100E000UL)
#define TMR7_BASE              (0x5100F000UL)
#define TMR8_BASE              (0x51010000UL)
#define TMR9_BASE              (0x51011000UL)
#define TMR10_BASE             (0x51012000UL)
#define TMR11_BASE             (0x51013000UL)
#define TMR12_BASE             (0x51014000UL)
#define TMR13_BASE             (0x51015000UL)
#define TMR14_BASE             (0x51016000UL)
#define TMR15_BASE             (0x51017000UL)
#define LIN0_BASE              (0x52000000UL)
#define LIN1_BASE              (0x52001000UL)
#define LIN2_BASE              (0x52002000UL)
#define LIN3_BASE              (0x52003000UL)
#define LIN4_BASE              (0x52004000UL)
#define LIN5_BASE              (0x52005000UL)
#define LIN6_BASE              (0x52006000UL)
#define LIN7_BASE              (0x52007000UL)
#define SPI0_BASE              (0x52008000UL)
#define SPI1_BASE              (0x52009000UL)
#define SPI2_BASE              (0x5200A000UL)
#define SPI3_BASE              (0x5200B000UL)
#define SPI4_BASE              (0x5200C000UL)
#define SPI5_BASE              (0x5200D000UL)
#define SPI6_BASE              (0x5200E000UL)
#define SPI7_BASE              (0x5200F000UL)
#define UART0_BASE             (0x52010000UL)
#define UART1_BASE             (0x52011000UL)
#define UART2_BASE             (0x52012000UL)
#define UART3_BASE             (0x52013000UL)
#define UART4_BASE             (0x52014000UL)
#define UART5_BASE             (0x52015000UL)
#define UART6_BASE             (0x52016000UL)
#define UART7_BASE             (0x52017000UL)
#define I2C0_BASE              (0x52018000UL)
#define I2C1_BASE              (0x52019000UL)
#define I2C2_BASE              (0x5201A000UL)
#define I2C3_BASE              (0x5201B000UL)
#define MSC_BASE               (0x5201C000UL)





/*-- Device specific Interrupt Numbers ---------------------------------------*/
#define PLIC_IRQ_WDT                       1   /* Watchdog timer interrupt */
#define PLIC_IRQ_RCU                       2   /* Reset and clock unit interrupt */
#define PLIC_IRQ_RTC                       3   /* RTC combine interrupt */
#define PLIC_IRQ_FLASH                     4   /* FLASH (main array and data array) combine interrupt */
#define PLIC_IRQ_SRAM                      5   /* SRAM interrupt */
#define PLIC_IRQ_L2_ERROR                  6   /* L2 error interrupt */
#define PLIC_IRQ_SIU                       7   /* System Interconnect Unit interrupt */
#define PLIC_IRQ_GPIOA                     8   /* GPIO A interrupt */
#define PLIC_IRQ_GPIOB                     9   /* GPIO B interrupt */
#define PLIC_IRQ_GPIOC                     10  /* GPIO C interrupt */
#define PLIC_IRQ_GPIOD                     11  /* GPIO D interrupt */
#define PLIC_IRQ_GPIOE                     12  /* GPIO E interrupt */
#define PLIC_IRQ_GPIOF                     13  /* GPIO F interrupt */
#define PLIC_IRQ_GPIOG                     14  /* GPIO G interrupt */
#define PLIC_IRQ_ETH                       15  /* Ethernet interrupt */
#define PLIC_IRQ_TMR0                      16  /* Timer 0 combine interrupt */
#define PLIC_IRQ_TMR1                      17  /* Timer 1 combine interrupt */
#define PLIC_IRQ_TMR2                      18  /* Timer 2 combine interrupt */
#define PLIC_IRQ_TMR3                      19  /* Timer 3 combine interrupt */
#define PLIC_IRQ_TMR4                      20  /* Timer 4 combine interrupt */
#define PLIC_IRQ_TMR5                      21  /* Timer 5 combine interrupt */
#define PLIC_IRQ_TMR6                      22  /* Timer 6 combine interrupt */
#define PLIC_IRQ_TMR7                      23  /* Timer 7 combine interrupt */
#define PLIC_IRQ_TMR8                      24  /* Timer 8 combine interrupt */
#define PLIC_IRQ_TMR9                      25  /* Timer 9 combine interrupt */
#define PLIC_IRQ_TMR10                     26  /* Timer 10 combine interrupt */
#define PLIC_IRQ_TMR11                     27  /* Timer 11 combine interrupt */
#define PLIC_IRQ_TMR12                     28  /* Timer 12 combine interrupt */
#define PLIC_IRQ_TMR13                     29  /* Timer 13 combine interrupt */
#define PLIC_IRQ_TMR14                     30  /* Timer 14 combine interrupt */
#define PLIC_IRQ_TMR15                     31  /* Timer 15 combine interrupt */
#define PLIC_IRQ_DMA_CH0                   32  /* DMA channel 0 interrupt */
#define PLIC_IRQ_DMA_CH1                   33  /* DMA channel 1 interrupt */
#define PLIC_IRQ_DMA_CH2                   34  /* DMA channel 2 interrupt */
#define PLIC_IRQ_DMA_CH3                   35  /* DMA channel 3 interrupt */
#define PLIC_IRQ_DMA_CH4                   36  /* DMA channel 4 interrupt */
#define PLIC_IRQ_DMA_CH5                   37  /* DMA channel 5 interrupt */
#define PLIC_IRQ_DMA_CH6                   38  /* DMA channel 6 interrupt */
#define PLIC_IRQ_DMA_CH7                   39  /* DMA channel 7 interrupt */
#define PLIC_IRQ_DMA_CH8                   40  /* DMA channel 8 interrupt */
#define PLIC_IRQ_DMA_CH9                   41  /* DMA channel 9 interrupt */
#define PLIC_IRQ_DMA_CH10                  42  /* DMA channel 10 interrupt */
#define PLIC_IRQ_DMA_CH11                  43  /* DMA channel 11 interrupt */
#define PLIC_IRQ_DMA_CH12                  44  /* DMA channel 12 interrupt */
#define PLIC_IRQ_DMA_CH13                  45  /* DMA channel 13 interrupt */
#define PLIC_IRQ_DMA_CH14                  46  /* DMA channel 14 interrupt */
#define PLIC_IRQ_DMA_CH15                  47  /* DMA channel 15 interrupt */
#define PLIC_IRQ_DMA_CH16                  48  /* DMA channel 16 interrupt */
#define PLIC_IRQ_DMA_CH17                  49  /* DMA channel 17 interrupt */
#define PLIC_IRQ_DMA_CH18                  50  /* DMA channel 18 interrupt */
#define PLIC_IRQ_DMA_CH19                  51  /* DMA channel 19 interrupt */
#define PLIC_IRQ_DMA_CH20                  52  /* DMA channel 20 interrupt */
#define PLIC_IRQ_DMA_CH21                  53  /* DMA channel 21 interrupt */
#define PLIC_IRQ_DMA_CH22                  54  /* DMA channel 22 interrupt */
#define PLIC_IRQ_DMA_CH23                  55  /* DMA channel 23 interrupt */
#define PLIC_IRQ_DMA_CH24                  56  /* DMA channel 24 interrupt */
#define PLIC_IRQ_DMA_CH25                  57  /* DMA channel 25 interrupt */
#define PLIC_IRQ_DMA_CH26                  58  /* DMA channel 26 interrupt */
#define PLIC_IRQ_DMA_CH27                  59  /* DMA channel 27 interrupt */
#define PLIC_IRQ_DMA_CH28                  60  /* DMA channel 28 interrupt */
#define PLIC_IRQ_DMA_CH29                  61  /* DMA channel 29 interrupt */
#define PLIC_IRQ_DMA_CH30                  62  /* DMA channel 30 interrupt */
#define PLIC_IRQ_DMA_CH31                  63  /* DMA channel 31 interrupt */
#define PLIC_IRQ_UART0                     64  /* UART0 combine interrupt */
#define PLIC_IRQ_UART0_TD                  65  /* UART0 transmit done interrupt */
#define PLIC_IRQ_UART0_RX                  66  /* UART0 recieve interrupt */
#define PLIC_IRQ_UART1                     67  /* UART1 combine interrupt */
#define PLIC_IRQ_UART1_TD                  68  /* UART1 transmit done interrupt */
#define PLIC_IRQ_UART1_RX                  69  /* UART1 recieve interrupt */
#define PLIC_IRQ_UART2                     70  /* UART2 combine interrupt */
#define PLIC_IRQ_UART2_TD                  71  /* UART2 transmit done interrupt */
#define PLIC_IRQ_UART2_RX                  72  /* UART2 recieve interrupt */
#define PLIC_IRQ_UART3                     73  /* UART3 combine interrupt */
#define PLIC_IRQ_UART3_TD                  74  /* UART3 transmit done interrupt */
#define PLIC_IRQ_UART3_RX                  75  /* UART3 recieve interrupt */
#define PLIC_IRQ_UART4                     76  /* UART4 combine interrupt */
#define PLIC_IRQ_UART4_TD                  77  /* UART4 transmit done interrupt */
#define PLIC_IRQ_UART4_RX                  78  /* UART4 recieve interrupt */
#define PLIC_IRQ_UART5                     79  /* UART5 combine interrupt */
#define PLIC_IRQ_UART5_TD                  80  /* UART5 transmit done interrupt */
#define PLIC_IRQ_UART5_RX                  81  /* UART5 recieve interrupt */
#define PLIC_IRQ_UART6                     82  /* UART6 combine interrupt */
#define PLIC_IRQ_UART6_TD                  83  /* UART6 transmit done interrupt */
#define PLIC_IRQ_UART6_RX                  84  /* UART6 recieve interrupt */
#define PLIC_IRQ_UART7                     85  /* UART7 combine interrupt */
#define PLIC_IRQ_UART7_TD                  86  /* UART7 transmit done interrupt */
#define PLIC_IRQ_UART7_RX                  87  /* UART7 recieve interrupt */
#define PLIC_IRQ_TMR0_CC0                  88  /* Timer 0 CAPCOM 0 interrupt */
#define PLIC_IRQ_TMR0_CC1                  89  /* Timer 0 CAPCOM 1 interrupt */
#define PLIC_IRQ_TMR0_CC2                  90  /* Timer 0 CAPCOM 2 interrupt */
#define PLIC_IRQ_TMR0_CC3                  91  /* Timer 0 CAPCOM 3 interrupt */
#define PLIC_IRQ_TMR0_CC4                  92  /* Timer 0 CAPCOM 4 interrupt */
#define PLIC_IRQ_TMR0_CC5                  93  /* Timer 0 CAPCOM 5 interrupt */
#define PLIC_IRQ_TMR0_CC6                  94  /* Timer 0 CAPCOM 6 interrupt */
#define PLIC_IRQ_TMR0_CC7                  95  /* Timer 0 CAPCOM 7 interrupt */
#define PLIC_IRQ_TMR1_CC0                  96  /* Timer 1 CAPCOM 0 interrupt */
#define PLIC_IRQ_TMR1_CC1                  97  /* Timer 1 CAPCOM 1 interrupt */
#define PLIC_IRQ_TMR1_CC2                  98  /* Timer 1 CAPCOM 2 interrupt */
#define PLIC_IRQ_TMR1_CC3                  99  /* Timer 1 CAPCOM 3 interrupt */
#define PLIC_IRQ_TMR1_CC4                  100 /* Timer 1 CAPCOM 4 interrupt */
#define PLIC_IRQ_TMR1_CC5                  101 /* Timer 1 CAPCOM 5 interrupt */
#define PLIC_IRQ_TMR1_CC6                  102 /* Timer 1 CAPCOM 6 interrupt */
#define PLIC_IRQ_TMR1_CC7                  103 /* Timer 1 CAPCOM 7 interrupt */
#define PLIC_IRQ_TMR2_CC0                  104 /* Timer 2 CAPCOM 0 interrupt */
#define PLIC_IRQ_TMR2_CC1                  105 /* Timer 2 CAPCOM 1 interrupt */
#define PLIC_IRQ_TMR2_CC2                  106 /* Timer 2 CAPCOM 2 interrupt */
#define PLIC_IRQ_TMR2_CC3                  107 /* Timer 2 CAPCOM 3 interrupt */
#define PLIC_IRQ_TMR2_CC4                  108 /* Timer 2 CAPCOM 4 interrupt */
#define PLIC_IRQ_TMR2_CC5                  109 /* Timer 2 CAPCOM 5 interrupt */
#define PLIC_IRQ_TMR2_CC6                  110 /* Timer 2 CAPCOM 6 interrupt */
#define PLIC_IRQ_TMR2_CC7                  111 /* Timer 2 CAPCOM 7 interrupt */
#define PLIC_IRQ_TMR3_CC0                  112 /* Timer 3 CAPCOM 0 interrupt */
#define PLIC_IRQ_TMR3_CC1                  113 /* Timer 3 CAPCOM 1 interrupt */
#define PLIC_IRQ_TMR3_CC2                  114 /* Timer 3 CAPCOM 2 interrupt */
#define PLIC_IRQ_TMR3_CC3                  115 /* Timer 3 CAPCOM 3 interrupt */
#define PLIC_IRQ_TMR3_CC4                  116 /* Timer 3 CAPCOM 4 interrupt */
#define PLIC_IRQ_TMR3_CC5                  117 /* Timer 3 CAPCOM 5 interrupt */
#define PLIC_IRQ_TMR3_CC6                  118 /* Timer 3 CAPCOM 6 interrupt */
#define PLIC_IRQ_TMR3_CC7                  119 /* Timer 3 CAPCOM 7 interrupt */
#define PLIC_IRQ_SPI0                      120 /* SPI0 combine interrupt */
#define PLIC_IRQ_SPI0_RX                   121 /* SPI0 receive interrupt */
#define PLIC_IRQ_SPI0_TX                   122 /* SPI0 transmit interrupt */
#define PLIC_IRQ_SPI1                      123 /* SPI1 combine interrupt */
#define PLIC_IRQ_SPI1_RX                   124 /* SPI1 receive interrupt */
#define PLIC_IRQ_SPI1_TX                   125 /* SPI1 transmit interrupt */
#define PLIC_IRQ_SPI2                      126 /* SPI2 combine interrupt */
#define PLIC_IRQ_SPI2_RX                   127 /* SPI2 receive interrupt */
#define PLIC_IRQ_SPI2_TX                   128 /* SPI2 transmit interrupt */
#define PLIC_IRQ_SPI3                      129 /* SPI3 combine interrupt */
#define PLIC_IRQ_SPI3_RX                   130 /* SPI3 receive interrupt */
#define PLIC_IRQ_SPI3_TX                   131 /* SPI3 transmit interrupt */
#define PLIC_IRQ_SPI4                      132 /* SPI4 combine interrupt */
#define PLIC_IRQ_SPI4_RX                   133 /* SPI4 receive interrupt */
#define PLIC_IRQ_SPI4_TX                   134 /* SPI4 transmit interrupt */
#define PLIC_IRQ_SPI5                      135 /* SPI5 combine interrupt */
#define PLIC_IRQ_SPI5_RX                   136 /* SPI5 receive interrupt */
#define PLIC_IRQ_SPI5_TX                   137 /* SPI5 transmit interrupt */
#define PLIC_IRQ_SPI6                      138 /* SPI6 combine interrupt */
#define PLIC_IRQ_SPI6_RX                   139 /* SPI6 receive interrupt */
#define PLIC_IRQ_SPI6_TX                   140 /* SPI6 transmit interrupt */
#define PLIC_IRQ_SPI7                      141 /* SPI7 combine interrupt */
#define PLIC_IRQ_SPI7_RX                   142 /* SPI7 receive interrupt */
#define PLIC_IRQ_SPI7_TX                   143 /* SPI7 transmit interrupt */
#define PLIC_IRQ_LIN0                      144 /* LIN0 combine interrupt */
#define PLIC_IRQ_LIN1                      145 /* LIN1 combine interrupt */
#define PLIC_IRQ_LIN2                      146 /* LIN2 combine interrupt */
#define PLIC_IRQ_LIN3                      147 /* LIN3 combine interrupt */
#define PLIC_IRQ_LIN4                      148 /* LIN4 combine interrupt */
#define PLIC_IRQ_LIN5                      149 /* LIN5 combine interrupt */
#define PLIC_IRQ_LIN6                      150 /* LIN6 combine interrupt */
#define PLIC_IRQ_LIN7                      151 /* LIN7 combine interrupt */
#define PLIC_IRQ_TMR0_PERIOD               152 /* Timer 0 period interrupt */
#define PLIC_IRQ_TMR1_PERIOD               153 /* Timer 1 period interrupt */
#define PLIC_IRQ_TMR2_PERIOD               154 /* Timer 2 period interrupt */
#define PLIC_IRQ_TMR3_PERIOD               155 /* Timer 3 period interrupt */
#define PLIC_IRQ_I2C0                      156 /* I2C0 interrupt */
#define PLIC_IRQ_I2C1                      157 /* I2C1 interrupt */
#define PLIC_IRQ_I2C2                      158 /* I2C2 interrupt */
#define PLIC_IRQ_I2C3                      159 /* I2C3 interrupt */
#define PLIC_IRQ_I2S0_TX                   160 /* I2S0 transmit interrupt */
#define PLIC_IRQ_I2S0_RX                   161 /* I2S0 receive interrupt */
#define PLIC_IRQ_I2S1_TX                   162 /* I2S1 transmit interrupt */
#define PLIC_IRQ_I2S1_RX                   163 /* I2S1 receive interrupt */
#define PLIC_IRQ_USB0                      164 /* USB0 interrupt */
#define PLIC_IRQ_USB1                      165 /* USB1 interrupt */
#define PLIC_IRQ_QSPI0                     166 /* QSPI0 interrupt */
#define PLIC_IRQ_QSPI1                     167 /* QSPI1 interrupt */
#define PLIC_IRQ_Reserved0                 168 /* Reserved interrupt slot 0 */
#define PLIC_IRQ_Reserved1                 169 /* Reserved interrupt slot 1 */
#define PLIC_IRQ_CANFD0                    170 /* CANFD0 interrupt */
#define PLIC_IRQ_CANFD0_TIMER              171 /* CANFD0 Timer interrupt */
#define PLIC_IRQ_CANFD0_ECC                172 /* CANFD0 ECC Single/Double Error detection interrupt */
#define PLIC_IRQ_CANFD1                    173 /* CANFD1 interrupt */
#define PLIC_IRQ_CANFD1_TIMER              174 /* CANFD1 Timer interrupt */
#define PLIC_IRQ_CANFD1_ECC                175 /* CANFD1 ECC Single/Double Error detection interrupt */
#define PLIC_IRQ_CANFD2                    176 /* CANFD2 interrupt */
#define PLIC_IRQ_CANFD2_TIMER              177 /* CANFD2 Timer interrupt */
#define PLIC_IRQ_CANFD2_ECC                178 /* CANFD2 ECC Single/Double Error detection interrupt */
#define PLIC_IRQ_CANFD3                    179 /* CANFD3 interrupt */
#define PLIC_IRQ_CANFD3_TIMER              180 /* CANFD3 Timer interrupt */
#define PLIC_IRQ_CANFD3_ECC                181 /* CANFD3 ECC Single/Double Error detection interrupt */
#define PLIC_IRQ_CAN_0                     182 /* CAN_0 interrupt */
#define PLIC_IRQ_CAN_1                     183 /* CAN_1 interrupt */
#define PLIC_IRQ_CAN_2                     184 /* CAN_2 interrupt */
#define PLIC_IRQ_CAN_3                     185 /* CAN_3 interrupt */
#define PLIC_IRQ_CAN_4                     186 /* CAN_4 interrupt */
#define PLIC_IRQ_CAN_5                     187 /* CAN_5 interrupt */
#define PLIC_IRQ_CAN_6                     188 /* CAN_6 interrupt */
#define PLIC_IRQ_CAN_7                     189 /* CAN_7 interrupt */
#define PLIC_IRQ_CAN_8                     190 /* CAN_8 interrupt */
#define PLIC_IRQ_CAN_9                     191 /* CAN_9 interrupt */
#define PLIC_IRQ_CAN_10                    192 /* CAN_10 interrupt */
#define PLIC_IRQ_CAN_11                    193 /* CAN_11 interrupt */
#define PLIC_IRQ_CAN_12                    194 /* CAN_12 interrupt */
#define PLIC_IRQ_CAN_13                    195 /* CAN_13 interrupt */
#define PLIC_IRQ_CAN_14                    196 /* CAN_14 interrupt */
#define PLIC_IRQ_CAN_15                    197 /* CAN_15 interrupt */
#define PLIC_IRQ_ADC_SEQ0                  198 /* ADC sequencer 0 interrupt */
#define PLIC_IRQ_ADC_SEQ1                  199 /* ADC sequencer 1 interrupt */
#define PLIC_IRQ_ADC_SEQ2                  200 /* ADC sequencer 2 interrupt */
#define PLIC_IRQ_ADC_SEQ3                  201 /* ADC sequencer 3 interrupt */
#define PLIC_IRQ_ADC_SEQ4                  202 /* ADC sequencer 4 interrupt */
#define PLIC_IRQ_ADC_SEQ5                  203 /* ADC sequencer 5 interrupt */
#define PLIC_IRQ_ADC_SEQ6                  204 /* ADC sequencer 6 interrupt */
#define PLIC_IRQ_ADC_SEQ7                  205 /* ADC sequencer 7 interrupt */
#define PLIC_IRQ_ADC_DC                    206 /* ADC digital comparator interrupt */
#define PLIC_IRQ_ACMP0                     207 /* ACMP0  interrupt */
#define PLIC_IRQ_ACMP1                     208 /* ACMP1  interrupt */
#define PLIC_IRQ_ACMP2                     209 /* ACMP2  interrupt */
#define PLIC_IRQ_ACMP3                     210 /* ACMP3 interrupt */
#define PLIC_IRQ_TRNG                      211 /* TRNG interrupt */
#define PLIC_IRQ_HASH                      212 /* HASH interrupt */
#define PLIC_IRQ_CRYPTO                    213 /* CRYPTO interrupt */
#define PLIC_IRQ_RTC_ALARM1                214 /* RTC alarm 1 interrupt */
#define PLIC_IRQ_RTC_ALARM2                215 /* RTC alarm 2 interrupt */
#define PLIC_IRQ_RTC_BATTERY               216 /* RTC switch to battery power supply interrupt */
#define PLIC_IRQ_RTC_N_POWER               217 /* RTC switch to normal power supply interrupt */
#define PLIC_IRQ_RTC_OVERFLOW              218 /* RTC counter overflow interrupt */
#define PLIC_IRQ_PWMxTZ                    219 /* PWM TZ combine interrupt */
#define PLIC_IRQ_PWM0                      220 /* PWM0 interrupt */
#define PLIC_IRQ_PWM0_HD                   221 /* PWM0 HD interrupt */
#define PLIC_IRQ_PWM1                      222 /* PWM1 interrupt */
#define PLIC_IRQ_PWM1_HD                   223 /* PWM1 HD interrupt */
#define PLIC_IRQ_PWM2                      224 /* PWM2 interrupt */
#define PLIC_IRQ_PWM2_HD                   225 /* PWM2 HD interrupt */
#define PLIC_IRQ_PWM3                      226 /* PWM3 interrupt */
#define PLIC_IRQ_PWM3_HD                   227 /* PWM3 HD interrupt */
#define PLIC_IRQ_PWM4                      228 /* PWM4 interrupt */
#define PLIC_IRQ_PWM4_HD                   229 /* PWM4 HD interrupt */
#define PLIC_IRQ_PWM5                      230 /* PWM5 interrupt */
#define PLIC_IRQ_PWM5_HD                   231 /* PWM5 HD interrupt */
#define PLIC_IRQ_PWM6                      232 /* PWM6 interrupt */
#define PLIC_IRQ_PWM6_HD                   233 /* PWM6 HD interrupt */
#define PLIC_IRQ_PWM7                      234 /* PWM7 interrupt */
#define PLIC_IRQ_PWM7_HD                   235 /* PWM7 HD interrupt */
#define PLIC_IRQ_PWM8                      236 /* PWM8 interrupt */
#define PLIC_IRQ_PWM8_HD                   237 /* PWM8 HD interrupt */
#define PLIC_IRQ_PWM9                      238 /* PWM9 interrupt */
#define PLIC_IRQ_PWM9_HD                   239 /* PWM9 HD interrupt */
#define PLIC_IRQ_PWM10                     240 /* PWM10 interrupt */
#define PLIC_IRQ_PWM10_HD                  241 /* PWM10 HD interrupt */
#define PLIC_IRQ_PWM11                     242 /* PWM11 interrupt */
#define PLIC_IRQ_PWM11_HD                  243 /* PWM11 HD interrupt */
#define PLIC_IRQ_PWM12                     244 /* PWM12 interrupt */
#define PLIC_IRQ_PWM12_HD                  245 /* PWM12 HD interrupt */
#define PLIC_IRQ_PWM13                     246 /* PWM13 interrupt */
#define PLIC_IRQ_PWM13_HD                  247 /* PWM13 HD interrupt */
#define PLIC_IRQ_PWM14                     248 /* PWM14 interrupt */
#define PLIC_IRQ_PWM14_HD                  249 /* PWM14 HD interrupt */
#define PLIC_IRQ_PWM15                     250 /* PWM15 interrupt */
#define PLIC_IRQ_PWM15_HD                  251 /* PWM15 HD interrupt */
#define PLIC_IRQ_MSC                       252 /* MSC combined interrupt */
#define PLIC_IRQ_MSC_upstream              253 /* MSC upstream combined interrupt */
#define PLIC_IRQ_MSC_downstream            254  /* MSC downstream combined interrupt */


#endif /* BOARD_CONFIG_NIIET_K1921VG015_CHIP_H_ */
