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
/**
 * @brief  Initializes the ETH MSP.
 * @param  heth: ETH handle
 * @retval None
 */
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Ethernet pins configuration *****************************************/
	/* STM32F764i-disco & STM32F746G-disco */
	/*
	 RMII_REF_CLK ----------------------> PA1
	 RMII_MDIO -------------------------> PA2
	 RMII_MDC --------------------------> PC1
	 RMII_MII_CRS_DV -------------------> PA7
	 RMII_MII_RXD0 ---------------------> PC4
	 RMII_MII_RXD1 ---------------------> PC5
	 RMII_MII_RXER ---------------------> PG2
	 RMII_MII_TX_EN --------------------> PG11
	 RMII_MII_TXD0 ---------------------> PG13
	 RMII_MII_TXD1 ---------------------> PG14
	 */

	/* Ethernet pins configuration ************************************************/
	/* nucleo-f767zi */
	 /*
	 RMII_REF_CLK ----------------------> PA1
	 RMII_MDIO -------------------------> PA2
	 RMII_MDC --------------------------> PC1
	 RMII_MII_CRS_DV -------------------> PA7
	 RMII_MII_RXD0 ---------------------> PC4
	 RMII_MII_RXD1 ---------------------> PC5
	 RMII_MII_RXER ---------------------> PG2
	 RMII_MII_TX_EN --------------------> PG11
	 RMII_MII_TXD0 ---------------------> PG13
	 RMII_MII_TXD1 ---------------------> PB13
	 */

	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF11_ETH;

	/* Configure PA1, PA2 and PA7 */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

#if defined(STM32F767xx) && defined(USE_STM32F7XX_NUCLEO_144)
	/* Configure PB13 */
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif

	/* Configure PC1, PC4 and PC5 */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PG2, PG11, PG13 and PG14 */
	__HAL_RCC_GPIOG_CLK_ENABLE();
#if !defined(USE_STM32F7XX_NUCLEO_144)
	/* Configure PG2, PG11, PG13 and PG14 */
	GPIO_InitStructure.Pin =
			GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14;
#else
	  GPIO_InitStructure.Pin =  GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_13;
#endif
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

	/* Enable ETHERNET clock  */
	__HAL_RCC_ETH_CLK_ENABLE();

#if defined(STM32F767xx) && defined(USE_STM32F7XX_NUCLEO_144)
	/* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_4);
#endif

}
