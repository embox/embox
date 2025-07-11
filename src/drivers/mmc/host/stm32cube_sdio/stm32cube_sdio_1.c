/**
 * @file
 * @brief 
 * @author Anton Bondarev
 * @version 0.1
 * @date 10.07.2025
 */

#include <util/log.h>

#include <assert.h>

#include <util/macro.h>

#include <kernel/irq.h>

#include <drivers/gpio.h>

#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include <embox/unit.h>
#include <framework/mod/options.h>


EMBOX_UNIT_INIT(stm32cube_sdio_init);

extern const struct mmc_host_ops stm32cube_sdio_ops;

#define SDIO_ID               1

#define SDIO_INSTANCE         SDIO

#define SDIO_IRQ_NUM          MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_IRQ)
#define SDIO_CLK_ENABLE       MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_CLK_ENABLE)()

#define TRANSFER_CLK_DIV      MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_CLK_DEF_TRANSFER_DIV)

#define SDIO_D0_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D0_PORT)
#define SDIO_D0_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D0_NR)
#define SDIO_D0_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D0_AF)
#define SDIO_D1_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D1_PORT)
#define SDIO_D1_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D1_NR)
#define SDIO_D1_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D1_AF)
#define SDIO_D2_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D2_PORT)
#define SDIO_D2_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D2_NR)
#define SDIO_D2_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D2_AF)
#define SDIO_D3_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D3_PORT)
#define SDIO_D3_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D3_NR)
#define SDIO_D3_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_D3_AF)

#define SDIO_CK_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CK_PORT)
#define SDIO_CK_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CK_NR)
#define SDIO_CK_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CK_AF)

#define SDIO_CMD_PORT           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CMD_PORT)
#define SDIO_CMD_PIN            MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CMD_NR)
#define SDIO_CMD_FUNC           MACRO_CONCAT(MACRO_CONCAT(CONF_SDIO,SDIO_ID),_PIN_CMD_AF)

static int stm32cube_sdio_hw_init(void) {

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

static int stm32cube_sdio_init(void) {
	static SD_HandleTypeDef hsd;

	/* It seems that  we can't check directly
	 * if any card present, so we just assume
	 * that we have a single card number zero */
	struct mmc_host *mmc = mmc_alloc_host();

	mmc->ops = &stm32cube_sdio_ops;
	mmc->priv = &hsd;

	stm32cube_sdio_hw_init();

	/* USER CODE END SDIO_Init 1 */
	hsd.Instance = SDIO_INSTANCE;
	hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
	hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd.Init.ClockDiv = TRANSFER_CLK_DIV;
	/* USER CODE BEGIN SDIO_Init 2 */
	
	// Общая настройка.
	// https://imax9.narod.ru/publs/F407les04.html
  
	// Обход бага с 4-бита инитом.
	// https://community.st.com/t5/stm32cubemx-mcus/sdio-interface-not-working-in-4bits-with-stm32f4-firmware/td-p/591776
  
	if (HAL_SD_Init(&hsd) != HAL_OK)
	{
    	return -1;
	}

	if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK) {
		return -1;
	}


	mmc_scan(mmc);

	return 0;
}

static irq_return_t stm32cube_sdio_irq_handler(unsigned int irq_num, void *arg) {
	(void)irq_num;
	(void)arg;
	return 0;
}

STATIC_IRQ_ATTACH(SDIO_IRQ_NUM, stm32cube_sdio_irq_handler, NULL);