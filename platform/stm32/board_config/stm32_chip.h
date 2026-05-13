
#define PERIPH_BASE           0x40000000UL /*!< Peripheral base address in the alias region      

/*!< Peripheral memory map */
#define APB1PERIPH_BASE       (PERIPH_BASE)
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x00010000UL)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x10000000UL)

/*!< APB1 peripherals */
#define TIM2_BASE             (APB1PERIPH_BASE + 0x0000UL)
#define TIM3_BASE             (APB1PERIPH_BASE + 0x0400UL)
#define TIM4_BASE             (APB1PERIPH_BASE + 0x0800UL)
#define TIM5_BASE             (APB1PERIPH_BASE + 0x0C00UL)
#define TIM6_BASE             (APB1PERIPH_BASE + 0x1000UL)
#define TIM7_BASE             (APB1PERIPH_BASE + 0x1400UL)
#define TIM12_BASE            (APB1PERIPH_BASE + 0x1800UL)
#define TIM13_BASE            (APB1PERIPH_BASE + 0x1C00UL)
#define TIM14_BASE            (APB1PERIPH_BASE + 0x2000UL)
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800UL)
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00UL)
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000UL)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800UL)
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00UL)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400UL)
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800UL)
#define UART4_BASE            (APB1PERIPH_BASE + 0x4C00UL)
#define UART5_BASE            (APB1PERIPH_BASE + 0x5000UL)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400UL)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800UL)
#define I2C3_BASE             (APB1PERIPH_BASE + 0x5C00UL)
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400UL)
#define CAN2_BASE             (APB1PERIPH_BASE + 0x6800UL)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000UL)
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400UL)

/*!< APB2 peripherals */
#define TIM1_BASE             (APB2PERIPH_BASE + 0x0000UL)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x0400UL)
#define USART1_BASE           (APB2PERIPH_BASE + 0x1000UL)
#define USART6_BASE           (APB2PERIPH_BASE + 0x1400UL)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2000UL)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2100UL)
#define ADC3_BASE             (APB2PERIPH_BASE + 0x2200UL)
#define ADC123_COMMON_BASE    (APB2PERIPH_BASE + 0x2300UL)
/* Legacy define */
#define ADC_BASE               ADC123_COMMON_BASE

#define SDIO_BASE             (APB2PERIPH_BASE + 0x2C00UL)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000UL)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800UL)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00UL)
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4000UL)
#define TIM10_BASE            (APB2PERIPH_BASE + 0x4400UL)
#define TIM11_BASE            (APB2PERIPH_BASE + 0x4800UL)

/*!< AHB1 peripherals */
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00UL)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000UL)
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400UL)
#define GPIOG_BASE            (AHB1PERIPH_BASE + 0x1800UL)
#define GPIOH_BASE            (AHB1PERIPH_BASE + 0x1C00UL)
#define GPIOI_BASE            (AHB1PERIPH_BASE + 0x2000UL)
#define CRC_BASE              (AHB1PERIPH_BASE + 0x3000UL)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800UL)
#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00UL)
#define DMA1_BASE             (AHB1PERIPH_BASE + 0x6000UL)
#define DMA1_Stream0_BASE     (DMA1_BASE + 0x010UL)
#define DMA1_Stream1_BASE     (DMA1_BASE + 0x028UL)
#define DMA1_Stream2_BASE     (DMA1_BASE + 0x040UL)
#define DMA1_Stream3_BASE     (DMA1_BASE + 0x058UL)
#define DMA1_Stream4_BASE     (DMA1_BASE + 0x070UL)
#define DMA1_Stream5_BASE     (DMA1_BASE + 0x088UL)
#define DMA1_Stream6_BASE     (DMA1_BASE + 0x0A0UL)
#define DMA1_Stream7_BASE     (DMA1_BASE + 0x0B8UL)
#define DMA2_BASE             (AHB1PERIPH_BASE + 0x6400UL)
#define DMA2_Stream0_BASE     (DMA2_BASE + 0x010UL)
#define DMA2_Stream1_BASE     (DMA2_BASE + 0x028UL)
#define DMA2_Stream2_BASE     (DMA2_BASE + 0x040UL)
#define DMA2_Stream3_BASE     (DMA2_BASE + 0x058UL)
#define DMA2_Stream4_BASE     (DMA2_BASE + 0x070UL)
#define DMA2_Stream5_BASE     (DMA2_BASE + 0x088UL)
#define DMA2_Stream6_BASE     (DMA2_BASE + 0x0A0UL)
#define DMA2_Stream7_BASE     (DMA2_BASE + 0x0B8UL)
#define ETH_BASE              (AHB1PERIPH_BASE + 0x8000UL)
#define ETH_MAC_BASE          (ETH_BASE)
#define ETH_MMC_BASE          (ETH_BASE + 0x0100UL)
#define ETH_PTP_BASE          (ETH_BASE + 0x0700UL)
#define ETH_DMA_BASE          (ETH_BASE + 0x1000UL)


#define WWDG_IRQn                    0       /*!< Window WatchDog Interrupt                                         */
#define PVD_IRQn                     1       /*!< PVD through EXTI Line detection Interrupt                         */
#define TAMP_STAMP_IRQn              2       /*!< Tamper and TimeStamp interrupts through the EXTI line             */
#define RTC_WKUP_IRQn                3       /*!< RTC Wakeup interrupt through the EXTI line                        */
#define FLASH_IRQn                   4       /*!< FLASH global Interrupt                                            */
#define RCC_IRQn                     5       /*!< RCC global Interrupt                                              */
#define EXTI0_IRQn                   6       /*!< EXTI Line0 Interrupt                                              */
#define EXTI1_IRQn                   7       /*!< EXTI Line1 Interrupt                                              */
#define EXTI2_IRQn                   8       /*!< EXTI Line2 Interrupt                                              */
#define EXTI3_IRQn                   9       /*!< EXTI Line3 Interrupt                                              */
#define EXTI4_IRQn                   1       /*!< EXTI Line4 Interrupt                                              */
#define DMA1_Stream0_IRQn            11      /*!< DMA1 Stream 0 global Interrupt                                    */
#define DMA1_Stream1_IRQn            12      /*!< DMA1 Stream 1 global Interrupt                                    */
#define DMA1_Stream2_IRQn            13      /*!< DMA1 Stream 2 global Interrupt                                    */
#define DMA1_Stream3_IRQn            14      /*!< DMA1 Stream 3 global Interrupt                                    */
#define DMA1_Stream4_IRQn            15      /*!< DMA1 Stream 4 global Interrupt                                    */
#define DMA1_Stream5_IRQn            16      /*!< DMA1 Stream 5 global Interrupt                                    */
#define DMA1_Stream6_IRQn            17      /*!< DMA1 Stream 6 global Interrupt                                    */
#define ADC_IRQn                     18      /*!< ADC1, ADC2 and ADC3 global Interrupts                             */
#define CAN1_TX_IRQn                 19      /*!< CAN1 TX Interrupt                                                 */
#define CAN1_RX0_IRQn                20      /*!< CAN1 RX0 Interrupt                                                */
#define CAN1_RX1_IRQn                21      /*!< CAN1 RX1 Interrupt                                                */
#define CAN1_SCE_IRQn                22      /*!< CAN1 SCE Interrupt                                                */
#define EXTI9_5_IRQn                 23      /*!< External Line[9:5] Interrupts                                     */
#define TIM1_BRK_TIM9_IRQn           24      /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
#define TIM1_UP_TIM10_IRQn           25      /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
#define TIM1_TRG_COM_TIM11_IRQn      26      /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
#define TIM1_CC_IRQn                 27      /*!< TIM1 Capture Compare Interrupt                                    */
#define TIM2_IRQn                    28      /*!< TIM2 global Interrupt                                             */
#define TIM3_IRQn                    29      /*!< TIM3 global Interrupt                                             */
#define TIM4_IRQn                    30      /*!< TIM4 global Interrupt                                             */
#define I2C1_EV_IRQn                 31      /*!< I2C1 Event Interrupt                                              */
#define I2C1_ER_IRQn                 32      /*!< I2C1 Error Interrupt                                              */
#define I2C2_EV_IRQn                 33      /*!< I2C2 Event Interrupt                                              */
#define I2C2_ER_IRQn                 34      /*!< I2C2 Error Interrupt                                              */
#define SPI1_IRQn                    35      /*!< SPI1 global Interrupt                                             */
#define SPI2_IRQn                    36      /*!< SPI2 global Interrupt                                             */
#define USART1_IRQn                  37      /*!< USART1 global Interrupt                                           */
#define USART2_IRQn                  38      /*!< USART2 global Interrupt                                           */
#define USART3_IRQn                  39      /*!< USART3 global Interrupt                                           */
#define EXTI15_10_IRQn               40      /*!< External Line[15:10] Interrupts                                   */
#define RTC_Alarm_IRQn               41      /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
#define OTG_FS_WKUP_IRQn             42      /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
#define TIM8_BRK_TIM12_IRQn          43      /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
#define TIM8_UP_TIM13_IRQn           44      /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
#define TIM8_TRG_COM_TIM14_IRQn      45      /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
#define TIM8_CC_IRQn                 46      /*!< TIM8 Capture Compare global interrupt                             */
#define DMA1_Stream7_IRQn            47      /*!< DMA1 Stream7 Interrupt                                            */
#define FSMC_IRQn                    48      /*!< FSMC global Interrupt                                             */
#define SDIO_IRQn                    49      /*!< SDIO global Interrupt                                             */
#define TIM5_IRQn                    50      /*!< TIM5 global Interrupt                                             */
#define SPI3_IRQn                    51      /*!< SPI3 global Interrupt                                             */
#define UART4_IRQn                   52      /*!< UART4 global Interrupt                                            */
#define UART5_IRQn                   53      /*!< UART5 global Interrupt                                            */
#define TIM6_DAC_IRQn                54      /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
#define TIM7_IRQn                    55      /*!< TIM7 global interrupt                                             */
#define DMA2_Stream0_IRQn            56      /*!< DMA2 Stream 0 global Interrupt                                    */
#define DMA2_Stream1_IRQn            57      /*!< DMA2 Stream 1 global Interrupt                                    */
#define DMA2_Stream2_IRQn            58      /*!< DMA2 Stream 2 global Interrupt                                    */
#define DMA2_Stream3_IRQn            59      /*!< DMA2 Stream 3 global Interrupt                                    */
#define DMA2_Stream4_IRQn            60      /*!< DMA2 Stream 4 global Interrupt                                    */
#define CAN2_TX_IRQn                 63      /*!< CAN2 TX Interrupt                                                 */
#define CAN2_RX0_IRQn                64      /*!< CAN2 RX0 Interrupt                                                */
#define CAN2_RX1_IRQn                65      /*!< CAN2 RX1 Interrupt                                                */
#define CAN2_SCE_IRQn                66      /*!< CAN2 SCE Interrupt                                                */
#define OTG_FS_IRQn                  67      /*!< USB OTG FS global Interrupt                                       */
#define DMA2_Stream5_IRQn            68      /*!< DMA2 Stream 5 global interrupt                                    */
#define DMA2_Stream6_IRQn            69      /*!< DMA2 Stream 6 global interrupt                                    */
#define DMA2_Stream7_IRQn            70      /*!< DMA2 Stream 7 global interrupt                                    */
#define USART6_IRQn                  71      /*!< USART6 global interrupt                                           */
#define I2C3_EV_IRQn                 72      /*!< I2C3 event interrupt                                              */
#define I2C3_ER_IRQn                 73      /*!< I2C3 error interrupt                                              */
#define OTG_HS_EP1_OUT_IRQn          74      /*!< USB OTG HS End Point 1 Out global interrupt                       */
#define OTG_HS_EP1_IN_IRQn           75      /*!< USB OTG HS End Point 1 In global interrupt                        */
#define OTG_HS_WKUP_IRQn             76      /*!< USB OTG HS Wakeup through EXTI interrupt                          */
#define OTG_HS_IRQn                  77      /*!< USB OTG HS global interrupt                                       */
#define RNG_IRQn                     80      /*!< RNG global Interrupt                                              */
#define FPU_IRQn                     81      /*!< FPU global interrupt                                               */