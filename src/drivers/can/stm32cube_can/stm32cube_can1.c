/**
 *
 * @author Anton Bondarev
 * @date 28.05.26
 */

#include <errno.h>
#include <linux/can.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/can_dev.h>

#include <bsp/stm32cube_hal.h>

#include <config/board_config.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#include "stm32cube_can_priv.h"

#define CAN_DEV_ID    1

EMBOX_UNIT_INIT(stm32cube_can_init);

extern const struct can_ops stm32cube_can_ops;

static struct stm32cube_can_priv can1_priv;

CAN_DEVICE_DEF(stm32cube_can_controller, &stm32cube_can_ops, &can1_priv, CAN_DEV_ID);

void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* CAN1 Periph clock enable */
  __HAL_RCC_CAN1_CLK_ENABLE();
  /* Enable GPIO clock ****************************************/

  /*##-2- Configure peripheral GPIO ##########################################*/ 
  /* CAN1 TX GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate =  GPIO_AF9_CAN1;
  
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* CAN1 RX GPIO pin configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate =  GPIO_AF9_CAN1;
  
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static int stm32cube_can_test_loopback(struct stm32cube_can_priv *priv) {
    uint8_t               TxData[8];
    uint8_t               RxData[8];
    uint32_t              TxMailbox;

      /*##-4- Start the Transmission process #####################################*/
  priv->TxHeader.StdId = 0x11;
  //TxHeader.RTR = CAN_RTR_DATA;
  //TxHeader.IDE = CAN_ID_STD;
  priv->TxHeader.DLC = 2;
  //TxHeader.TransmitGlobalTime = DISABLE;
  TxData[0] = 0xCA;
  TxData[1] = 0xFE;
  
  /* Request transmission */
  if(HAL_CAN_AddTxMessage(&priv->CanHandle, &priv->TxHeader, TxData, &TxMailbox) != HAL_OK)
  {
    /* Transmission request Error */
    return 0;
  }
  
  /* Wait transmission complete */
  while(HAL_CAN_GetTxMailboxesFreeLevel(&priv->CanHandle) != 3) {}

  /*##-5- Start the Reception process ########################################*/
  if(HAL_CAN_GetRxFifoFillLevel(&priv->CanHandle, CAN_RX_FIFO0) != 1)
  {
    /* Reception Missing */
    return 0;
  }

  if(HAL_CAN_GetRxMessage(&priv->CanHandle, CAN_RX_FIFO0, &priv->RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    return 0;
  }

    return 0;
}

static int stm32cube_can_init(void) {

    struct stm32cube_can_priv *priv = &can1_priv;

  /*##-1- Configure the CAN peripheral #######################################*/
    priv->CanHandle.Instance = CAN1;
    
    priv->CanHandle.Init.TimeTriggeredMode = DISABLE;
    priv->CanHandle.Init.AutoBusOff = DISABLE;
    priv->CanHandle.Init.AutoWakeUp = DISABLE;
    priv->CanHandle.Init.AutoRetransmission = ENABLE;
    priv->CanHandle.Init.ReceiveFifoLocked = DISABLE;
    priv->CanHandle.Init.TransmitFifoPriority = DISABLE;
    priv->CanHandle.Init.Mode = CAN_MODE_LOOPBACK;
    priv->CanHandle.Init.SyncJumpWidth = CAN_SJW_1TQ;
    priv->CanHandle.Init.TimeSeg1 = CAN_BS1_6TQ;
    priv->CanHandle.Init.TimeSeg2 = CAN_BS2_8TQ;
    priv->CanHandle.Init.Prescaler = 2;

    if(HAL_CAN_Init(&priv->CanHandle) != 0) {
        return 0;
    }
  
      /*##-2- Configure the CAN Filter ###########################################*/
  priv->sFilterConfig.FilterBank = 0;
  priv->sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  priv->sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  priv->sFilterConfig.FilterIdHigh = 0x0000;
  priv->sFilterConfig.FilterIdLow = 0x0000;
  priv->sFilterConfig.FilterMaskIdHigh = 0x0000;
  priv->sFilterConfig.FilterMaskIdLow = 0x0000;
  priv->sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  priv->sFilterConfig.FilterActivation = ENABLE;
  priv->sFilterConfig.SlaveStartFilterBank = 14;

  if(HAL_CAN_ConfigFilter(&priv->CanHandle, &priv->sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    return 0;
  }

    /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&priv->CanHandle) != HAL_OK)
  {
    /* Start Error */
        return 0;
  }
  priv->TxHeader.RTR = CAN_RTR_DATA;
  priv->TxHeader.IDE = CAN_ID_STD;
  priv->TxHeader.TransmitGlobalTime = DISABLE;
  stm32cube_can_test_loopback(priv);

	return 0;
}