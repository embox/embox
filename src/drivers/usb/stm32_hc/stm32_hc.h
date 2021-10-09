/**
 * @file
 * @brief
 *
 * @author  Filipp Chubukov
 * @date    04.05.2021
 */
#ifndef SRC_DRIVERS_USB_STM32_HC_STM32_HC3_H_
#define SRC_DRIVERS_USB_STM32_HC_STM32_HC3_H_

#if defined(STM32F429xx) || defined(STM32F407xx)

#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_hcd.h>

#else
#error "don't support platform"
#endif

#if defined(STM32F429xx)
#define STM32_HC_VBUS_PORT            GPIOG
#define STM32_HC_VBUS_PIN             GPIO_PIN_6
#define STM32_HC_PORT_VBUS_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE
#elif defined(STM32F407xx)
#define STM32_HC_VBUS_PORT            GPIOC
#define STM32_HC_VBUS_PIN             GPIO_PIN_0
#define STM32_HC_PORT_VBUS_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE
#else
#error "don't support platform"
#endif


/* STM32 USB Port states */
#define STM32_PORT_IDLE					0
#define STM32_PORT_CONNECTED			1
#define STM32_PORT_ENABLED				2
#define STM32_PORT_READY				3

/* STM32 URB Direction */
#define STM32_URB_IN					0
#define STM32_URB_OUT 					1

/* STM32 USB Pipes */
#define STM32_PIPE_CONTROL_IN			1
#define STM32_PIPE_CONTROL_OUT			0
#define STM32_PIPE_BULK_IN				3
#define STM32_PIPE_BULK_OUT				2

#define STM32_MAX_PACKET_SIZE			64
#define STM32_USB_DEV_ADDR 				0

extern HCD_HandleTypeDef stm32_hcd_handler;

struct usb_hcd;

struct stm32_endp {
	uint8_t pipe_idx;
	uint8_t endp_type;
	uint8_t endp_addr;
	uint8_t endp_dir;
};


struct stm32_hcd {
	struct usb_hcd *hcd;
	HCD_HandleTypeDef *hhcd;
	uint8_t port_status;
	uint8_t free_chan_idx;
	struct stm32_endp *ctlr_endp_in;
	struct stm32_endp *ctlr_endp_out;
};


static inline struct stm32_hcd *hhcd2stm_hcd(HCD_HandleTypeDef *hhcd) {
	return (struct stm32_hcd *)stm32_hcd_handler.pData;
}

#endif /* SRC_DRIVERS_USB_STM32_HC_STM32_HC3_H_ */
