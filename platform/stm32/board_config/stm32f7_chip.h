/** @addtogroup Peripheral_memory_map
  * @{
  */
#define RAMITCM_BASE           0x00000000UL /*!< Base address of : 16KB RAM reserved for CPU execution/instruction accessible over ITCM  */
#define FLASHITCM_BASE         0x00200000UL /*!< Base address of : (up to 2 MB) embedded FLASH memory  accessible over ITCM              */
#define FLASHAXI_BASE          0x08000000UL /*!< Base address of : (up to 2 MB) embedded FLASH memory accessible over AXI                */
#define RAMDTCM_BASE           0x20000000UL /*!< Base address of : 128KB system data RAM accessible over DTCM                            */
#define PERIPH_BASE            0x40000000UL /*!< Base address of : AHB/ABP Peripherals                                                   */
#define BKPSRAM_BASE           0x40024000UL /*!< Base address of : Backup SRAM(4 KB)                                                     */
#define QSPI_BASE              0x90000000UL /*!< Base address of : QSPI memories  accessible over AXI                                    */
#define FMC_R_BASE             0xA0000000UL /*!< Base address of : FMC Control registers                                                 */
#define QSPI_R_BASE            0xA0001000UL /*!< Base address of : QSPI Control  registers                                               */
#define SRAM1_BASE             0x20020000UL /*!< Base address of : 368KB RAM1 accessible over AXI/AHB                                    */
#define SRAM2_BASE             0x2007C000UL /*!< Base address of : 16KB RAM2 accessible over AXI/AHB                                     */
#define FLASH_END              0x081FFFFFUL /*!< FLASH end address */
#define FLASH_OTP_BASE         0x1FF0F000UL /*!< Base address of : (up to 1024 Bytes) embedded FLASH OTP Area                            */
#define FLASH_OTP_END          0x1FF0F41FUL /*!< End address of : (up to 1024 Bytes) embedded FLASH OTP Area                             */

/* Legacy define */
#define FLASH_BASE     FLASHAXI_BASE

/*!< Peripheral memory map */
#define APB1PERIPH_BASE        PERIPH_BASE
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
#define LPTIM1_BASE           (APB1PERIPH_BASE + 0x2400UL)
#define RTC_BASE              (APB1PERIPH_BASE + 0x2800UL)
#define WWDG_BASE             (APB1PERIPH_BASE + 0x2C00UL)
#define IWDG_BASE             (APB1PERIPH_BASE + 0x3000UL)
#define CAN3_BASE             (APB1PERIPH_BASE + 0x3400UL)
#define SPI2_BASE             (APB1PERIPH_BASE + 0x3800UL)
#define SPI3_BASE             (APB1PERIPH_BASE + 0x3C00UL)
#define SPDIFRX_BASE          (APB1PERIPH_BASE + 0x4000UL)
#define USART2_BASE           (APB1PERIPH_BASE + 0x4400UL)
#define USART3_BASE           (APB1PERIPH_BASE + 0x4800UL)
#define UART4_BASE            (APB1PERIPH_BASE + 0x4C00UL)
#define UART5_BASE            (APB1PERIPH_BASE + 0x5000UL)
#define I2C1_BASE             (APB1PERIPH_BASE + 0x5400UL)
#define I2C2_BASE             (APB1PERIPH_BASE + 0x5800UL)
#define I2C3_BASE             (APB1PERIPH_BASE + 0x5C00UL)
#define I2C4_BASE             (APB1PERIPH_BASE + 0x6000UL)
#define CAN1_BASE             (APB1PERIPH_BASE + 0x6400UL)
#define CAN2_BASE             (APB1PERIPH_BASE + 0x6800UL)
#define CEC_BASE              (APB1PERIPH_BASE + 0x6C00UL)
#define PWR_BASE              (APB1PERIPH_BASE + 0x7000UL)
#define DAC_BASE              (APB1PERIPH_BASE + 0x7400UL)
#define UART7_BASE            (APB1PERIPH_BASE + 0x7800UL)
#define UART8_BASE            (APB1PERIPH_BASE + 0x7C00UL)

/*!< APB2 peripherals */
#define TIM1_BASE             (APB2PERIPH_BASE + 0x0000UL)
#define TIM8_BASE             (APB2PERIPH_BASE + 0x0400UL)
#define USART1_BASE           (APB2PERIPH_BASE + 0x1000UL)
#define USART6_BASE           (APB2PERIPH_BASE + 0x1400UL)
#define SDMMC2_BASE           (APB2PERIPH_BASE + 0x1C00UL)
#define ADC1_BASE             (APB2PERIPH_BASE + 0x2000UL)
#define ADC2_BASE             (APB2PERIPH_BASE + 0x2100UL)
#define ADC3_BASE             (APB2PERIPH_BASE + 0x2200UL)
#define ADC_BASE              (APB2PERIPH_BASE + 0x2300UL)
#define SDMMC1_BASE           (APB2PERIPH_BASE + 0x2C00UL)
#define SPI1_BASE             (APB2PERIPH_BASE + 0x3000UL)
#define SPI4_BASE             (APB2PERIPH_BASE + 0x3400UL)
#define SYSCFG_BASE           (APB2PERIPH_BASE + 0x3800UL)
#define EXTI_BASE             (APB2PERIPH_BASE + 0x3C00UL)
#define TIM9_BASE             (APB2PERIPH_BASE + 0x4000UL)
#define TIM10_BASE            (APB2PERIPH_BASE + 0x4400UL)
#define TIM11_BASE            (APB2PERIPH_BASE + 0x4800UL)
#define SPI5_BASE             (APB2PERIPH_BASE + 0x5000UL)
#define SPI6_BASE             (APB2PERIPH_BASE + 0x5400UL)
#define SAI1_BASE             (APB2PERIPH_BASE + 0x5800UL)
#define SAI2_BASE             (APB2PERIPH_BASE + 0x5C00UL)
#define SAI1_Block_A_BASE     (SAI1_BASE + 0x004UL)
#define SAI1_Block_B_BASE     (SAI1_BASE + 0x024UL)
#define SAI2_Block_A_BASE     (SAI2_BASE + 0x004UL)
#define SAI2_Block_B_BASE     (SAI2_BASE + 0x024UL)
#define LTDC_BASE             (APB2PERIPH_BASE + 0x6800UL)
#define LTDC_Layer1_BASE      (LTDC_BASE + 0x0084UL)
#define LTDC_Layer2_BASE      (LTDC_BASE + 0x0104UL)
#define DSI_BASE              (APB2PERIPH_BASE + 0x6C00UL)
#define DFSDM1_BASE           (APB2PERIPH_BASE + 0x7400UL)
#define DFSDM1_Channel0_BASE  (DFSDM1_BASE + 0x00UL)
#define DFSDM1_Channel1_BASE  (DFSDM1_BASE + 0x20UL)
#define DFSDM1_Channel2_BASE  (DFSDM1_BASE + 0x40UL)
#define DFSDM1_Channel3_BASE  (DFSDM1_BASE + 0x60UL)
#define DFSDM1_Channel4_BASE  (DFSDM1_BASE + 0x80UL)
#define DFSDM1_Channel5_BASE  (DFSDM1_BASE + 0xA0UL)
#define DFSDM1_Channel6_BASE  (DFSDM1_BASE + 0xC0UL)
#define DFSDM1_Channel7_BASE  (DFSDM1_BASE + 0xE0UL)
#define DFSDM1_Filter0_BASE   (DFSDM1_BASE + 0x100UL)
#define DFSDM1_Filter1_BASE   (DFSDM1_BASE + 0x180UL)
#define DFSDM1_Filter2_BASE   (DFSDM1_BASE + 0x200UL)
#define DFSDM1_Filter3_BASE   (DFSDM1_BASE + 0x280UL)
#define MDIOS_BASE            (APB2PERIPH_BASE + 0x7800UL)
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
#define GPIOJ_BASE            (AHB1PERIPH_BASE + 0x2400UL)
#define GPIOK_BASE            (AHB1PERIPH_BASE + 0x2800UL)
#define CRC_BASE              (AHB1PERIPH_BASE + 0x3000UL)
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800UL)
#define FLASH_R_BASE          (AHB1PERIPH_BASE + 0x3C00UL)
#define UID_BASE              0x1FF0F420UL                   /*!< Unique device ID register base address */
#define FLASHSIZE_BASE        0x1FF0F442UL                   /*!< FLASH Size register base address */
#define PACKAGE_BASE          0x1FF0F7E0UL                   /*!< Package size register base address */
/* Legacy define */
#define PACKAGESIZE_BASE      PACKAGE_BASE

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
#define DMA2D_BASE            (AHB1PERIPH_BASE + 0xB000UL)
/*!< AHB2 peripherals */
#define DCMI_BASE             (AHB2PERIPH_BASE + 0x50000UL)
#define JPEG_BASE           (AHB2PERIPH_BASE + 0x51000UL)
#define CRYP_BASE             (AHB2PERIPH_BASE + 0x60000UL)
#define HASH_BASE             (AHB2PERIPH_BASE + 0x60400UL)
#define HASH_DIGEST_BASE      (AHB2PERIPH_BASE + 0x60710UL)
#define RNG_BASE              (AHB2PERIPH_BASE + 0x60800UL)
/*!< FMC Bankx registers base address */
#define FMC_Bank1_R_BASE      (FMC_R_BASE + 0x0000UL)
#define FMC_Bank1E_R_BASE     (FMC_R_BASE + 0x0104UL)
#define FMC_Bank3_R_BASE      (FMC_R_BASE + 0x0080UL)
#define FMC_Bank5_6_R_BASE    (FMC_R_BASE + 0x0140UL)

/* Debug MCU registers base address */
#define DBGMCU_BASE           0xE0042000UL

/*!< USB registers base address */
#define USB_OTG_HS_PERIPH_BASE               0x40040000UL
#define USB_OTG_FS_PERIPH_BASE               0x50000000UL

#define USB_OTG_GLOBAL_BASE                  0x0000UL
#define USB_OTG_DEVICE_BASE                  0x0800UL
#define USB_OTG_IN_ENDPOINT_BASE             0x0900UL
#define USB_OTG_OUT_ENDPOINT_BASE            0x0B00UL
#define USB_OTG_EP_REG_SIZE                  0x0020UL
#define USB_OTG_HOST_BASE                    0x0400UL
#define USB_OTG_HOST_PORT_BASE               0x0440UL
#define USB_OTG_HOST_CHANNEL_BASE            0x0500UL
#define USB_OTG_HOST_CHANNEL_SIZE            0x0020UL
#define USB_OTG_PCGCCTL_BASE                 0x0E00UL
#define USB_OTG_FIFO_BASE                    0x1000UL
#define USB_OTG_FIFO_SIZE                    0x1000UL