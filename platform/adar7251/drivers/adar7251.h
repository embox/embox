

#ifndef PLATFORM_ADAR7251_ADAR7251_H_
#define PLATFORM_ADAR7251_ADAR7251_H_


#define CLK_CTRL                    0x0000u
#define CLK_BYPASS_PLL              0x0001u
#define CLK_USE_PLL                 0x0000u

#define PLL_DEN                     0x0001u
#define PLL_NUM                     0x0002u

#define PLL_CTRL                    0x0003u
#define PLL_EN                      0x0001u
#define PLL_DIS                     0x0000u
#define PLL_TYPE_INT                0x0000u
#define PLL_TYPE_FRAKT              0x0001u << 1
#define PLL_INPUT_PRESC_MASK        0x00f0u
#define PLL_INPUT_PRESC_SHIFT       4
#define PLL_INTEGER_DIV_MASK        0xf800
#define PLL_INTEGER_DIV_SHIFT       11

#define PLL_LOCK                    0x0005u //RO

#define MASTER_ENABLE               0x0040u
#define MASTER_EN                   0x0001u
#define MASTER_DIS                  0x0000u

#define ADC_ENABLE                  0x0041u
#define ADC1_EN                     0x0001u
#define ADC2_EN                     0x0002u
#define ADC3_EN                     0x0004u
#define ADC4_EN                     0x0008u
#define LN_PG1_EN                   0x0010u
#define LN_PG2_EN                   0x0020u
#define LN_PG3_EN                   0x0040u
#define LN_PG4_EN                   0x0080u

#define POWER_ENABLE                0x0042u
#define CLOCK_LOSS_EN               0x0001u << 9
#define FLASH_LDO_EN                0x0001u << 7
#define LDO_EN                      0x0001u << 6
#define AUXADC_EN                   0x0001u << 5
#define MP_EN                       0x0001u << 4
#define DIN_EN                      0x0001u << 3
#define POUT_EN                     0x0001u << 2
#define SOUT_EN                     0x0001u << 1
#define CLKGEN_EN                   0x0001u << 0

#define PGA_GAIN        0x0101u
#define PGA_GAIN_1_4    0x0000u
#define PGA_GAIN_2_8    0x0001u
#define PGA_GAIN_5_6    0x0002u
#define PGA_GAIN_11_2   0x0003u
#define PGA_1_OFFSET    0x0000u
#define PGA_2_OFFSET    0x0002u
#define PGA_3_OFFSET    0x0004u
#define PGA_4_OFFSET    0x0006u

#define ADC_ROUTING1_4              0x0102u
// ADC_SRC
#define ADC_DISABLED                0x0000u
#define LNA_PGA_EQ                  0x0001u
#define LNA_PGA                     0x0002u
#define LNA_PGA_EQ_BP               0x0003u
#define CH_SWAP                     0x0004u
#define TEST_PIN                    0X0005u
#define ADC1_OFFSET                 0
#define ADC2_OFFSET                 4
#define ADC3_OFFSET                 8
#define ADC4_OFFSET                 12

#define DECIM_RATE        0x0140u
#define RATE_1M8          0x0002u
#define RATE_1M2          0x0003u
#define RATE_900K         0x0004u
#define RATE_600K         0x0005u
#define RATE_450K         0x0006u
#define RATE_300K         0x0007u

#define HIGH_PASS           0x0141u
#define ENABLE_HP           0x0001u
#define DISABLE_HP          0x0000u
#define HP_SHIFT_11         0x000Bu << 1
#define HP_SHIFT_12         0x000Cu << 1
#define HP_SHIFT_13         0x000Du << 1
#define HP_SHIFT_14         0x000Eu << 1
#define HP_SHIFT_15         0x000Fu << 1
#define HP_SHIFT_16         0x0010u << 1
#define HP_SHIFT_17         0x0011u << 1
#define HP_SHIFT_18         0x0012u << 1
#define PHASE_EQ_EN         0x0001u << 6
#define PHASE_EQ_DIS        0x0000u

#define OUTPUT_MODE             0x01C2u
#define OUT_MODE_SERIAL         0x0000u
#define OUT_MODE_PARALLEL       0x0001u
#define CONV_START_EN           0x0000u
#define CONV_START_DIS          0x0002u

#define SERIAL_MODE                 0x01C0u
#define TDM_MODE_2PF                0x0000u
#define TDM_MODE_4PF                0x0001u
#define DATA_LJF                    0x0000u
#define DATA_I2S                    0x0001u << 2
#define BCLK_POL_POS                0x0001u << 3
#define BCLK_POL_NEG                0x0000u
#define LRCLK_POL_POS               0x0001u << 4
#define LRCLK_POL_NEG               0x0000u
#define LRCLK_MODE_PULSE            0x0001u << 5
#define LRCLK_MODE_50X50            0x0000u
#define SCLK_MASTER                 0x0001u << 6
#define SCLK_SLAVE                  0x0000u

#define SCLK_ADC_PIN                0x0288u
#define ADC_DOUT0_PIN               0x0289u
#define ADC_DOUT1_PIN               0x028Au
#define FS_ADC_PIN                  0x0286u

#define PIN_PULLDOWN_EN             0x0004u
#define PIN_PULLDOWN_DIS            0x0000u

#define DRIVE_LOWEST                0x0000u
#define DRIVE_LOW                   0x0001u
#define DRIVE_HI                    0x0002u
#define DRIVE_HIGHEST               0x0003u


#define ADAR_SPI_READ       0x0001u
#define ADAR_SPI_WRITE      0x0000u
#define ADAR_SPI_ADDR_0     0x0000u
#define ADAR_SPI_ADDR_1     0x0002u
#define ADAR1_R             (ADAR_SPI_READ  | ADAR_SPI_ADDR_1)
#define ADAR1_W             (ADAR_SPI_WRITE | ADAR_SPI_ADDR_1)
#define ADAR0_R             (ADAR_SPI_READ  | ADAR_SPI_ADDR_0)
#define ADAR0_W             (ADAR_SPI_WRITE | ADAR_SPI_ADDR_0)

#define CRC_EN          0xFD00u
#define CRC_DISABLE     0x0001u
#define CRC_DISABLE_CRC 0x3307u  //0xC86C
#define CRC_ENABLE      0X0000u


#define ASIL_CLEAR            0x0080u
#define ASIL_CLEAR_ERR        0x0001u

// hi/low byte macro
#define UpperByte(x)  ((0xff00u & x) >> 8)
#define LowerByte(x) (0x00ffu & x)

#endif /* PLATFORM_ADAR7251_ADAR7251_H_ */
