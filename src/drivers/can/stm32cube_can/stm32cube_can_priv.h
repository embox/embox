/**
 * @file
 *
 * @date 28.05.2026
 * @author: Anton Bondarev
 */

#ifndef SRC_DRIVERS_CAN_STM32CUBE_CAN_PRIV_H_
#define SRC_DRIVERS_CAN_STM32CUBE_CAN_PRIV_H_


#include <stdint.h>

#include <bsp/stm32cube_hal.h>

struct pin_description;

struct stm32cube_can_priv {
	const struct pin_description *pin_desc;
	const uintptr_t               base_addr;
	const char                   *clk_name;
	int                           idx;

    CAN_HandleTypeDef             CanHandle;
    CAN_TxHeaderTypeDef           TxHeader;
    CAN_RxHeaderTypeDef           RxHeader;
	CAN_FilterTypeDef             sFilterConfig;
};

#endif /* SRC_DRIVERS_CAN_STM32CUBE_CAN_PRIV_H_ */
