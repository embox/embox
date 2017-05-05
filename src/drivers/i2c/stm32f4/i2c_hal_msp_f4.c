/**
 * @file
 * @brief
 *
 * @date    29.03.2017
 * @author  Alex Kalmuk
 */

/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards_ComPolling/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.2.6
  * @date    06-May-2016
  * @brief   HAL MSP module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "stm32_i2c_conf_f4.h"
#include <util/log.h>
#include <kernel/irq.h>

static irq_return_t i2c_ev_irq_handler(unsigned int irq_nr, void *data) {
	I2C_HandleTypeDef *I2cHandle = (I2C_HandleTypeDef *) data;
	HAL_I2C_EV_IRQHandler(I2cHandle);
	return IRQ_HANDLED;
}

static irq_return_t i2c_er_irq_handler(unsigned int irq_nr, void *data) {
	I2C_HandleTypeDef *I2cHandle = (I2C_HandleTypeDef *) data;
	HAL_I2C_ER_IRQHandler(I2cHandle);
	return IRQ_HANDLED;
}

/**
  * @brief I2C MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
	int res;

	GPIO_InitTypeDef GPIO_InitStruct;

	log_info(">>> HAL_I2C_MspInit\n");

	/*##-1- Enable GPIO Clocks #################################################*/
	/* Enable GPIO TX/RX clock */
	I2Cx_SCL_GPIO_CLK_ENABLE();
	I2Cx_SDA_GPIO_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* I2C TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = I2Cx_SCL_AF;
	HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);

	/* I2C RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
	GPIO_InitStruct.Alternate = I2Cx_SDA_AF;
	HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);

	/*##-3- Enable I2C peripheral Clock ########################################*/
	/* Enable I2C1 clock */
	I2Cx_CLK_ENABLE();

	res = irq_attach(I2Cx_EV_IRQn, i2c_ev_irq_handler, 0, hi2c, "I2C events");
	if (res < 0) {
		log_error(">>> HAL_I2C_MspInit error irq_attach\n");
	}
	res = irq_attach(I2Cx_ER_IRQn, i2c_er_irq_handler, 0, hi2c, "I2C errors");
	if (res < 0) {
		log_error(">>> HAL_I2C_MspInit error irq_attach\n");
	}
	log_info(">>> HAL_I2C_MspInit finished\n");
}
