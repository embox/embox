/**
 * @file
 * @brief 
 * @author Anton Bondarev
 * @version 0.1
 * @date 10.07.2025
 */

#include <util/log.h>

#include <assert.h>

#include <drivers/gpio.h>

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include <framework/mod/options.h>


#define SDIO_IRQ_NUM          CONF_SDIO2_IRQ
#define SDIO_INSTANCE         SDMMC2
#define SDIO_CLK_ENABLE       CONF_SDIO2_CLK_ENABLE()

#define TRANSFER_CLK_DIV      CONF_SDIO2_CLK_DEF_TRANSFER_DIV


#define SDIO_D0_PORT           CONF_SDIO2_PIN_D0_PORT
#define SDIO_D0_PIN            CONF_SDIO2_PIN_D0_NR
#define SDIO_D0_FUNC           CONF_SDIO2_PIN_D0_AF
#define SDIO_D1_PORT           CONF_SDIO2_PIN_D1_PORT
#define SDIO_D1_PIN            CONF_SDIO2_PIN_D1_NR
#define SDIO_D1_FUNC           CONF_SDIO2_PIN_D1_AF
#define SDIO_D2_PORT           CONF_SDIO2_PIN_D2_PORT
#define SDIO_D2_PIN            CONF_SDIO2_PIN_D2_NR
#define SDIO_D2_FUNC           CONF_SDIO2_PIN_D2_AF
#define SDIO_D3_PORT           CONF_SDIO2_PIN_D3_PORT
#define SDIO_D3_PIN            CONF_SDIO2_PIN_D3_NR
#define SDIO_D3_FUNC           CONF_SDIO2_PIN_D3_AF

#define SDIO_CK_PORT           CONF_SDIO2_PIN_CK_PORT
#define SDIO_CK_PIN            CONF_SDIO2_PIN_CK_NR
#define SDIO_CK_FUNC           CONF_SDIO2_PIN_CK_AF

#define SDIO_CMD_PORT           CONF_SDIO2_PIN_CMD_PORT
#define SDIO_CMD_PIN            CONF_SDIO2_PIN_CMD_NR
#define SDIO_CMD_FUNC           CONF_SDIO2_PIN_CMD_AF

#if (OPTION_GET(NUMBER,hw_init) != 0)
int stm32cube_sdio_hw_init(void) {

	/* SDIO clock enable */
 	SDIO_CLK_ENABLE();

	/*********************/

	gpio_setup_mode(SDIO_D0_PORT, 1 << SDIO_D0_PIN,
		GPIO_MODE_ALT_SET(SDIO_D0_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(SDIO_D1_PORT, 1 << SDIO_D1_PIN,
		GPIO_MODE_ALT_SET(SDIO_D1_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(SDIO_D2_PORT, 1 << SDIO_D2_PIN,
		GPIO_MODE_ALT_SET(SDIO_D2_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(SDIO_D3_PORT, 1 << SDIO_D3_PIN,
		GPIO_MODE_ALT_SET(SDIO_D3_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(SDIO_CK_PORT, 1 << SDIO_CK_PIN,
		GPIO_MODE_ALT_SET(SDIO_CK_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(SDIO_CMD_PORT, 1 << SDIO_CMD_PIN,
		GPIO_MODE_ALT_SET(SDIO_CMD_FUNC) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	return 0;

}
#endif /* (OPTION_GET(NUMBER,hw_init) != 0) */