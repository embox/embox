/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <drivers/diag.h>

//#include <drivers/sensors/vl53l0x.h>

#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"

#include "vl53l0x_def.h"
#include "vl53l0x_api.h"
#include "vl53l0x_tof.h"

#define PROXIMITY_I2C_ADDRESS         ((uint16_t)0x0052)
#define VL53L0X_ID                    ((uint16_t)0xEEAA)
#define VL53L0X_XSHUT_Pin GPIO_PIN_6
#define VL53L0X_XSHUT_GPIO_Port GPIOC

static void VL53L0X_PROXIMITY_MspInit(void);
static uint16_t VL53L0X_PROXIMITY_GetDistance(void);
static int VL53L0X_PROXIMITY_Init(void);
extern void SENSOR_IO_Init(void);

extern I2C_HandleTypeDef hI2cHandler;
static VL53L0X_Dev_t Dev =
{
  .I2cHandle = &hI2cHandler,
  .I2cDevAddr = PROXIMITY_I2C_ADDRESS
};

int main(int argc, char **argv) {
	printf("Testing vl53l0x sensor output\n");

	if (VL53L0X_PROXIMITY_Init()) {
		printf("Failed to init VL53L0X!\n");
		return -1;
	}

	while (!diag_kbhit()) {
		int x = VL53L0X_PROXIMITY_GetDistance();
		printf("  Proximity: %d mm     \r", x);
		fflush(stdout);
		usleep(500*1000);
	}
	printf("\n");

	return 0;
}

static int VL53L0X_PROXIMITY_Init(void)
{
  uint16_t vl53l0x_id = 0; 
  VL53L0X_DeviceInfo_t VL53L0X_DeviceInfo;
  
  /* Initialize IO interface */
  SENSOR_IO_Init();  
  VL53L0X_PROXIMITY_MspInit();
  
  memset(&VL53L0X_DeviceInfo, 0, sizeof(VL53L0X_DeviceInfo_t));
  
  if (VL53L0X_ERROR_NONE != VL53L0X_GetDeviceInfo(&Dev, &VL53L0X_DeviceInfo)) {
    printf("VL53L0X: Failed to get infos!\n");
    return -1;
  }  
  if (VL53L0X_ERROR_NONE != VL53L0X_RdWord(&Dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, (uint16_t *) &vl53l0x_id)) {
    printf("VL53L0X: Failed to Initialize!\n");
    return -1;
  }
  if (vl53l0x_id != VL53L0X_ID) { 
    printf("VL53L0X: Failed to send its ID!\n");
    return -1;
  }
  if (VL53L0X_ERROR_NONE != VL53L0X_DataInit(&Dev)) {
    printf("VL53L0X_DataInit() failed!\n");
    return -1;
  }
  Dev.Present = 1;
  SetupSingleShot(Dev);
  return 0;
}

static void VL53L0X_PROXIMITY_MspInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  /*Configure GPIO pin : VL53L0X_XSHUT_Pin */
  GPIO_InitStruct.Pin = VL53L0X_XSHUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(VL53L0X_XSHUT_GPIO_Port, &GPIO_InitStruct);
  
  HAL_GPIO_WritePin(VL53L0X_XSHUT_GPIO_Port, VL53L0X_XSHUT_Pin, GPIO_PIN_SET);
  
  HAL_Delay(1000);  
}

static uint16_t VL53L0X_PROXIMITY_GetDistance(void)
{
  VL53L0X_RangingMeasurementData_t RangingMeasurementData;
  
  VL53L0X_PerformSingleRangingMeasurement(&Dev, &RangingMeasurementData);
  
  return RangingMeasurementData.RangeMilliMeter;  
}

