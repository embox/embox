/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.08.2014
 */

#include <util/log.h>

#include <drivers/net/stm32cube_eth.h>

/*****************************************************************************
 Ethernet MSP Routines
 *****************************************************************************/

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOs clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /* Configure PA1, PA2 , PA7 */
  GPIO_InitStructure.Pin =  GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL ;
  GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 ;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

  GPIO_InitStructure.Pin =  GPIO_PIN_2 | GPIO_PIN_3;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);

  GPIO_InitStructure.Pin =  GPIO_PIN_10;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStructure);

  /* Enable the Ethernet global Interrupt */
  HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
  HAL_NVIC_EnableIRQ(ETH_IRQn);

  /* Enable Ethernet clocks */
  __HAL_RCC_ETH1MAC_CLK_ENABLE();
  __HAL_RCC_ETH1TX_CLK_ENABLE();
  __HAL_RCC_ETH1RX_CLK_ENABLE();
}

