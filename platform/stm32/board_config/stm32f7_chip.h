#include <stm32_chip.h>

/*!< APB1 peripherals */
#define LPTIM1_BASE           (APB1PERIPH_BASE + 0x2400UL)
#define CAN3_BASE             (APB1PERIPH_BASE + 0x3400UL)
#define SPDIFRX_BASE          (APB1PERIPH_BASE + 0x4000UL)
#define I2C4_BASE             (APB1PERIPH_BASE + 0x6000UL)
#define CEC_BASE              (APB1PERIPH_BASE + 0x6C00UL)
#define UART7_BASE            (APB1PERIPH_BASE + 0x7800UL)
#define UART8_BASE            (APB1PERIPH_BASE + 0x7C00UL)

/*!< APB2 peripherals */
#define SDMMC2_BASE           (APB2PERIPH_BASE + 0x1C00UL)
#define SDMMC1_BASE           (APB2PERIPH_BASE + 0x2C00UL)
#define SPI4_BASE             (APB2PERIPH_BASE + 0x3400UL)
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
#define GPIOJ_BASE            (AHB1PERIPH_BASE + 0x2400UL)
#define GPIOK_BASE            (AHB1PERIPH_BASE + 0x2800UL)

#define UID_BASE              0x1FF0F420UL                   /*!< Unique device ID register base address */
#define FLASHSIZE_BASE        0x1FF0F442UL                   /*!< FLASH Size register base address */
#define PACKAGE_BASE          0x1FF0F7E0UL                   /*!< Package size register base address */
/* Legacy define */
#define PACKAGESIZE_BASE      PACKAGE_BASE

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