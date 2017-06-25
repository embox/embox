/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.08.2014
 */

#include <util/log.h>

#include <drivers/net/stm32cube_eth.h>

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth) {
	/*(##) Enable the Ethernet interface clock using
	 (+++) __HAL_RCC_ETHMAC_CLK_ENABLE();
	 (+++) __HAL_RCC_ETHMACTX_CLK_ENABLE();
	 (+++) __HAL_RCC_ETHMACRX_CLK_ENABLE();

	 (##) Initialize the related GPIO clocks
	 (##) Configure Ethernet pin-out
	 (##) Configure Ethernet NVIC interrupt (IT mode)
	 */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable ETHERNET clock
	__HAL_RCC_ETHMAC_CLK_ENABLE();
	__HAL_RCC_ETHMACTX_CLK_ENABLE();
	__HAL_RCC_ETHMACRX_CLK_ENABLE();
	*/
	__HAL_RCC_ETH_CLK_ENABLE();

	/* Enable GPIOs clocks */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	/* Ethernet pins configuration ************************************************/
	/*
	 ETH_MDIO --------------> PA2
	 ETH_MDC ---------------> PC1

	 ETH_RMII_REF_CLK-------> PA1

	 ETH_RMII_CRS_DV -------> PA7
	 ETH_MII_RX_ER   -------> PB10
	 ETH_RMII_RXD0   -------> PC4
	 ETH_RMII_RXD1   -------> PC5
	 ETH_RMII_TX_EN  -------> PB11
	 ETH_RMII_TXD0   -------> PB12
	 ETH_RMII_TXD1   -------> PB13

	 ETH_RST_PIN     -------> PE2
	 */

	/* Configure PA1,PA2 and PA7 */
	GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure PB10,PB11,PB12 and PB13 */
	GPIO_InitStructure.Pin =
			GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12	| GPIO_PIN_13;
	/* GPIO_InitStructure.Alternate = GPIO_AF11_ETH; */
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure PC1, PC4 and PC5 */
	GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
	/* GPIO_InitStructure.Alternate = GPIO_AF11_ETH; */
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	if (heth->Init.MediaInterface == ETH_MEDIA_INTERFACE_MII) {
		/* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
		HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
	}

	/* Configure the PHY RST  pin */
	GPIO_InitStructure.Pin = GPIO_PIN_2;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_Delay(1);
}
