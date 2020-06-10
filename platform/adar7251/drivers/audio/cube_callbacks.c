/*
 * cube_fixups.c
 *
 *  Created on: Jun 3, 2020
 *      Author: anton
 */

#include <util/log.h>
#include <stddef.h>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_dma.h>

#include <kernel/thread.h>
#include <kernel/sched.h>

#include "adar_sai.h"

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hsai);

	if (!sai_device.sai_active) {
		return;
	}
	if (sai_device.sai_cur_buf) {
		log_error("!!!!!!!!!!!!!!!!SAI OVERFLOW!!!!!!!!!!!!!!!");
		return;
	}
	sai_device.sai_cur_buf = (void *)&sai_device.sai_buf[SAI_SAMPLES_BUFFER / 2];

	sched_wakeup(&sai_device.sai_thread->schedee);
}

/**
 * @brief Rx Transfer half completed callback.
 * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
 *                the configuration information for SAI module.
 * @retval None
 */
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hsai);

	if (!sai_device.sai_active) {
		return;
	}
	if (sai_device.sai_cur_buf) {
		log_error("!!!!!!!!!!!!!!!!SAI OVERFLOW!!!!!!!!!!!!!!!");
		return;
	}
	sai_device.sai_cur_buf = (void *) &sai_device.sai_buf[0];
	sched_wakeup(&sai_device.sai_thread->schedee);
}

/**
 * @brief SAI error callback.
 * @param  hsai: pointer to a SAI_HandleTypeDef structure that contains
 *                the configuration information for SAI module.
 * @retval None
 */
void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
/* Prevent unused argument(s) compilation warning */
UNUSED(hsai);
	log_error("!!!!!!!!!!!!!!");

/* NOTE : This function should not be modified, when the callback is needed,
          the HAL_SAI_ErrorCallback could be implemented in the user file
 */
}
