#include <gen_board_conf.h>



#define CLIC_US_IRQn                  0     /*!< User Software Interrupt */
#define CLIC_Reserved0                1     /*!< Reserved */
#define CLIC_Reserved1                2     /*!< Reserved */
#define CLIC_MS_IRQn                  3     /*!< Machine Software Interrupt */
#define CLIC_UT_IRQn                  4     /*!< User Timer Interrupt */
#define CLIC_Reserved2                5     /*!< Reserved */
#define CLIC_Reserved3                6     /*!< Reserved */
#define CLIC_MT_IRQn                  7     /*!< Machine Timer Interrupt */
#define CLIC_Reserved4                8     /*!< Reserved */
#define CLIC_Reserved5                9     /*!< Reserved */
#define CLIC_Reserved6                10    /*!< Reserved */
#define CLIC_Reserved7                11    /*!< Reserved */
#define CLIC_CS_IRQn                  12    /*!< CLIC Software Interrupt */
#define CLIC_Reserved8                13    /*!< Reserved */
#define CLIC_Reserved9                14    /*!< Reserved */
#define CLIC_Reserved10               15    /*!< Reserved */
#define CLIC_QSPI0_IRQn               16    /*!< QSPI0 Interrupt */
#define CLIC_UART0_IRQn               17    /*!< UART0 Interrupt */
#define CLIC_UART1_IRQn               18    /*!< UART1 Interrupt */
#define CLIC_UART2_IRQn               19    /*!< UART2 Interrupt */
#define CLIC_I2C0_IRQn                20    /*!< I2C0 Interrupt */
#define CLIC_I2C1_IRQn                21    /*!< I2C1 Interrupt */
#define CLIC_I2S0_IRQn                22    /*!< I2S0 Interrupt */
#define CLIC_TIM0_Channel0_IRQn       23    /*!< TIM0 Channel 0 Interrupt */
#define CLIC_TIM0_Channel1_IRQn       24    /*!< TIM0 Channel 1 Interrupt */
#define CLIC_TIM0_Channel2_IRQn       25    /*!< TIM0 Channel 2 Interrupt */
#define CLIC_TIM0_Channel3_IRQn       26    /*!< TIM0 Channel 3 Interrupt */
#define CLIC_WDT0_IRQn                27    /*!< WDT0 Interrupt */
#define CLIC_GPIO0_IRQn               28    /*!< GPIO0 Interrupt */
#define CLIC_PWMG0_IRQn               29    /*!< PWMG0 Interrupt */
#define CLIC_SPI0_IRQn                30    /*!< SPI0 Interrupt */
#define CLIC_SPI1_IRQn                31    /*!< SPI1 Interrupt */
#define CLIC_CANFD0_IRQn              32    /*!< CANFD0 Interrupt */
#define CLIC_QSPI1_IRQn               33    /*!< QSPI1 Interrupt */
#define CLIC_UART3_IRQn               34    /*!< UART3 Interrupt */
#define CLIC_UART4_IRQn               35    /*!< UART4 Interrupt */
#define CLIC_UART5_IRQn               36    /*!< UART5 Interrupt */
#define CLIC_I2C2_IRQn                37    /*!< I2C2 Interrupt */
#define CLIC_I2C3_IRQn                38    /*!< I2C3 Interrupt */
#define CLIC_I2S1_IRQn                39    /*!< I2S1 Interrupt */
#define CLIC_TIM1_Channel0_IRQn       40    /*!< TIM1 Channel 0 Interrupt */
#define CLIC_TIM1_Channel1_IRQn       41    /*!< TIM1 Channel 1 Interrupt */
#define CLIC_TIM1_Channel2_IRQn       42    /*!< TIM1 Channel 2 Interrupt */
#define CLIC_TIM1_Channel3_IRQn       43    /*!< TIM1 Channel 3 Interrupt */
#define CLIC_Reserved11               44    /*!< Reserved */
#define CLIC_GPIO1_IRQn               45    /*!< GPIO1 Interrupt */
#define CLIC_PWMG1_IRQn               46    /*!< PWMG1 Interrupt */
#define CLIC_SPI2_IRQn                47    /*!< SPI2 Interrupt */
#define CLIC_SPI3_IRQn                48    /*!< SPI3 Interrupt */
#define CLIC_CANFD1_IRQn              49    /*!< CANFD1 Interrupt */
#define CLIC_ADC0_IRQn                50    /*!< ADC0 Interrupt */
#define CLIC_ADC1_IRQn                51    /*!< ADC1 Interrupt */
#define CLIC_ADC2_IRQn                52    /*!< ADC2 Interrupt */
#define CLIC_PWMA0_IRQn               53    /*!< PWMA0 Interrupt */
#define CLIC_QE0_IRQn                 54    /*!< QE0 Interrupt */
#define CLIC_PWMA1_IRQn               55    /*!< PWMA1 Interrupt */
#define CLIC_QE1_IRQn                 56    /*!< QE1 Interrupt */
#define CLIC_PWMA2_IRQn               57    /*!< PWMA2 Interrupt */
#define CLIC_QE2_IRQn                 58    /*!< QE2 Interrupt */
#define CLIC_PWMA3_IRQn               59    /*!< PWMA3 Interrupt */
#define CLIC_QE3_IRQn                 60    /*!< QE3 Interrupt */
#define CLIC_GPIO2_IRQn               61    /*!< GPIO2 Interrupt */
#define CLIC_UART6_IRQn               62    /*!< UART6 Interrupt */
#define CLIC_UART7_IRQn               63    /*!< UART7 Interrupt */
#define CLIC_USB_IRQn                 64    /*!< USB Interrupt */
#define CLIC_DMA0_IRQn                65    /*!< DMA0 Interrupt */
#define CLIC_DMA1_IRQn                66    /*!< DMA1 Interrupt */
#define CLIC_DMA2_IRQn                67    /*!< DMA2 Interrupt */
#define CLIC_CORE2_IRQn               68    /*!< CORE2 Interrupt */
#define CLIC_EXTI_PA_IRQn             69    /*!< EXTI PA Interrupt */
#define CLIC_EXTI_PB_IRQn             70    /*!< EXTI PB Interrupt */
#define CLIC_EXTI_PC_IRQn             71    /*!< EXTI PC Interrupt */
#define CLIC_CLKI_Mon_IRQn            72    /*!< CLKI Monitor Interrupt */
#define CLIC_CORE0_BusError_IRQn      73    /*!< CORE0 Bus Access Error Interrupt */
#define CLIC_CORE1_BusError_IRQn      74    /*!< CORE1 Bus Access Error Interrupt */


/* Peripheral memory map */
#define PERIPH_BASE             0x11000000UL                        /*!< Peripheral base address */

#define APB0PERIPH_BASE         PERIPH_BASE                         /*!< APB0 peripheral base address */
#define APB1PERIPH_BASE         (PERIPH_BASE + 0x02000000UL)        /*!< APB1 peripheral base address */
#define APB2PERIPH_BASE         (PERIPH_BASE + 0x03000000UL)        /*!< APB2 peripheral base address */
#define AHBPERIPH_BASE          (PERIPH_BASE + 0x04000000UL)        /*!< AHB peripheral base address */

/* APB0 periphery */
#define QSPI0_BASE              (APB0PERIPH_BASE + 0x00000000UL)    /*!< QSPI0 peripheral base address */
#define UART0_BASE              (APB0PERIPH_BASE + 0x00001000UL)    /*!< UART0 peripheral base address */
#define UART1_BASE              (APB0PERIPH_BASE + 0x00002000UL)    /*!< UART1 peripheral base address */
#define UART2_BASE              (APB0PERIPH_BASE + 0x00003000UL)    /*!< UART2 peripheral base address */
#define I2C0_BASE               (APB0PERIPH_BASE + 0x00004000UL)    /*!< I2C0 peripheral base address */
#define I2C1_BASE               (APB0PERIPH_BASE + 0x00005000UL)    /*!< I2C1 peripheral base address */
#define I2S0_BASE               (APB0PERIPH_BASE + 0x00006000UL)    /*!< I2S0 peripheral base address */
#define TIM0_BASE               (APB0PERIPH_BASE + 0x00007000UL)    /*!< TIM0 peripheral base address */
#define WDT0_BASE               (APB0PERIPH_BASE + 0x00008000UL)    /*!< WDT0 peripheral base address */
#define GPIO0_BASE              (APB0PERIPH_BASE + 0x00009000UL)    /*!< GPIO0 peripheral base address */
#define PWMG0_BASE              (APB0PERIPH_BASE + 0x0000A000UL)    /*!< PWMG0 peripheral base address */
#define SPI0_BASE               (APB0PERIPH_BASE + 0x0000B000UL)    /*!< SPI0 peripheral base address */
#define SPI1_BASE               (APB0PERIPH_BASE + 0x0000C000UL)    /*!< SPI1 peripheral base address */
#define CANFD0_BASE             (APB0PERIPH_BASE + 0x00010000UL)    /*!< CANFD0 peripheral base address */

/* APB1 periphery */
#define QSPI1_BASE              (APB1PERIPH_BASE + 0x00000000UL)    /*!< QSPI1 peripheral base address */
#define UART3_BASE              (APB1PERIPH_BASE + 0x00001000UL)    /*!< UART3 peripheral base address */
#define UART4_BASE              (APB1PERIPH_BASE + 0x00002000UL)    /*!< UART4 peripheral base address */
#define UART5_BASE              (APB1PERIPH_BASE + 0x00003000UL)    /*!< UART5 peripheral base address */
#define I2C2_BASE               (APB1PERIPH_BASE + 0x00004000UL)    /*!< I2C2 peripheral base address */
#define I2C3_BASE               (APB1PERIPH_BASE + 0x00005000UL)    /*!< I2C3 peripheral base address */
#define I2S1_BASE               (APB1PERIPH_BASE + 0x00006000UL)    /*!< I2S1 peripheral base address */
#define TIM1_BASE               (APB1PERIPH_BASE + 0x00007000UL)    /*!< TIM1 peripheral base address */
#define WDT1_BASE               (APB1PERIPH_BASE + 0x00008000UL)    /*!< WDT1 peripheral base address */
#define GPIO1_BASE              (APB1PERIPH_BASE + 0x00009000UL)    /*!< GPIO1 peripheral base address */
#define PWMG1_BASE              (APB1PERIPH_BASE + 0x0000A000UL)    /*!< PWMG1 peripheral base address */
#define SPI2_BASE               (APB1PERIPH_BASE + 0x0000B000UL)    /*!< SPI2 peripheral base address */
#define SPI3_BASE               (APB1PERIPH_BASE + 0x0000C000UL)    /*!< SPI3 peripheral base address */
#define CANFD1_BASE             (APB1PERIPH_BASE + 0x00010000UL)    /*!< CANFD1 peripheral base address */

/* APB2 periphery */
#define CRU_BASE                (APB2PERIPH_BASE + 0x00000000UL)    /*!< CRU peripheral base address */
#define ADC0_BASE               (APB2PERIPH_BASE + 0x00001000UL)    /*!< ADC0 peripheral base address */
#define ADC1_BASE               (APB2PERIPH_BASE + 0x00002000UL)    /*!< ADC1 peripheral base address */
#define ADC2_BASE               (APB2PERIPH_BASE + 0x00003000UL)    /*!< ADC2 peripheral base address */
#define PWMA0_BASE              (APB2PERIPH_BASE + 0x00004000UL)    /*!< PWMA0 peripheral base address */
#define QE0_BASE                (APB2PERIPH_BASE + 0x00004050UL)    /*!< QE0 peripheral base address */
#define PWMA1_BASE              (APB2PERIPH_BASE + 0x00005000UL)    /*!< PWMA1 peripheral base address */
#define QE1_BASE                (APB2PERIPH_BASE + 0x00005050UL)    /*!< QE1 peripheral base address */
#define PWMA2_BASE              (APB2PERIPH_BASE + 0x00006000UL)    /*!< PWMA2 peripheral base address */
#define QE2_BASE                (APB2PERIPH_BASE + 0x00006050UL)    /*!< QE2 peripheral base address */
#define PWMA3_BASE              (APB2PERIPH_BASE + 0x00007000UL)    /*!< PWMA3 peripheral base address */
#define QE3_BASE                (APB2PERIPH_BASE + 0x00007050UL)    /*!< QE3 peripheral base address */
#define GPIO2_BASE              (APB2PERIPH_BASE + 0x00008000UL)    /*!< GPIO2 peripheral base address */
#define UART6_BASE              (APB2PERIPH_BASE + 0x00009000UL)    /*!< UART6 peripheral base address */
#define UART7_BASE              (APB2PERIPH_BASE + 0x0000A000UL)    /*!< UART7 peripheral base address */
#define AXI_BASE                (APB2PERIPH_BASE + 0x0000B000UL)    /*!< AXI config base address */

/* AHB periphery */
#define USB_BASE                (AHBPERIPH_BASE + 0x00000000UL)     /*!< USB peripheral base address */
#define DMA0_BASE               (AHBPERIPH_BASE + 0x00100000UL)     /*!< DMA0 peripheral base address */
#define DMA1_BASE               (AHBPERIPH_BASE + 0x00101000UL)     /*!< DMA1 peripheral base address */
#define DMA2_BASE               (AHBPERIPH_BASE + 0x00102000UL)     /*!< DMA2 peripheral base address */
#define CORE2_INT_SYS_BASE      (AHBPERIPH_BASE + 0x001037F0UL)     /*!< CORE2 INT peripheral base address (System side) */
#define CORE2_PIO_SYS_BASE      (AHBPERIPH_BASE + 0x001037F4UL)     /*!< CORE2 PIO peripheral base address (System side) */
#define TRACER_ENC_BASE         (AHBPERIPH_BASE + 0x00104000UL)     /*!< Tracer encoder control interface base address */
#define TRACER_RAM_BASE         (AHBPERIPH_BASE + 0x00105000UL)     /*!< Tracer RAM control interface base address */
#define TRACER_PIB_BASE         (AHBPERIPH_BASE + 0x00106000UL)     /*!< Tracer PIB control interface base address */
#define TRACER_SRAM_BASE        (AHBPERIPH_BASE + 0x00107000UL)     /*!< Tracer SRAM interface base address */
#define AHB_BASE                (AHBPERIPH_BASE + 0x00108000UL)     /*!< AHB config base address */

/* CORE2 memory (System side) */
#define CORE2_TCMA_SYS_BASE     (AHBPERIPH_BASE + 0x00103000UL)     /*!< CORE2 TCMA base address (System side) */
#define CORE2_TCMB_SYS_BASE     (AHBPERIPH_BASE + 0x00103400UL)     /*!< CORE2 TCMB base address (System side) */

/* CORE2 periphery */
#define CORE2PERIPH_BASE        0x400007F0UL                        /*!< CORE2 peripheral base address (CORE2 side) */
#define CORE2_INT_BASE          (CORE2PERIPH_BASE + 0x00000000UL)   /*!< CORE2 INT peripheral base address (CORE2 side) */
#define CORE2_PIO_BASE          (CORE2PERIPH_BASE + 0x00000004UL)   /*!< CORE2 PIO peripheral base address (CORE2 side) */

/* System memory */
#define ROM0_BASE               0x40000000UL                        /*!< ROM0 base address */
#define ROM1_BASE               0x40008000UL                        /*!< ROM1 base address */
#define TCMA_BASE               0x40010000UL                        /*!< TCMA base address */
#define TCMB_BASE               0x40020000UL                        /*!< TCMB base address */
