/**
 * @file
 *
 * @date Aug 24, 2023
 * @author Anton Bondarev
 */

#include <util/log.h>

//#include <drivers/net/stm32cube_eth.h>
#include <bsp/stm32cube_hal.h>

#include <drivers/gpio.h>
#include <config/board_config.h>

#define ETH_GPIO_PORT(name)      MACRO_CONCAT(MACRO_CONCAT(CONF_ETH_PIN_,name),_PORT)
#define ETH_GPIO_PIN(name)       MACRO_CONCAT(MACRO_CONCAT(CONF_ETH_PIN_,name),_NR)
#define ETH_GPIO_AF(name)        MACRO_CONCAT(MACRO_CONCAT(CONF_ETH_PIN_,name),_AF)

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{

/* Ethernet pins configuration ************************************************/
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

 #if 0
 GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOs clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();



  /* Configure PA1, PA2 and PA7 */
  GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Alternate = GPIO_AF11_ETH;
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure PB13 */
  GPIO_InitStructure.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure PC1, PC4 and PC5 */
  GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PG2, PG11, PG13 and PG14 */
  GPIO_InitStructure.Pin =  GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

    /* Enable ETHERNET clock  */
  __HAL_RCC_ETH_CLK_ENABLE();
#else

/* RMII_REF_CLK */
	gpio_setup_mode(ETH_GPIO_PORT(RMII_REF_CLK),
			(1 << ETH_GPIO_PIN(RMII_REF_CLK)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_REF_CLK)) | GPIO_MODE_OUT_PUSH_PULL
		);
/* RMII_MDIO */
  gpio_setup_mode(ETH_GPIO_PORT(RMII_MDIO),
			(1 << ETH_GPIO_PIN(RMII_MDIO)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_MDIO)) | GPIO_MODE_OUT_PUSH_PULL
		);
/* RMII_MDC */
  gpio_setup_mode(ETH_GPIO_PORT(RMII_MDC),
			(1 << ETH_GPIO_PIN(RMII_MDC)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_MDC)) | GPIO_MODE_OUT_PUSH_PULL
		);
/* RMII_MII_CRS_DV */
  gpio_setup_mode(ETH_GPIO_PORT(RMII_MII_CRS_DV),
			(1 << ETH_GPIO_PIN(RMII_MII_CRS_DV)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_MII_CRS_DV)) | GPIO_MODE_OUT_PUSH_PULL
		);
/* RMII_MII_RXD0 */
  gpio_setup_mode(ETH_GPIO_PORT(RMII_MII_RXD0),
			(1 << ETH_GPIO_PIN(RMII_MII_RXD0)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_MII_RXD0)) | GPIO_MODE_OUT_PUSH_PULL
		);
/* RMII_MII_RXD1 */
  gpio_setup_mode(ETH_GPIO_PORT(RMII_MII_RXD1),
			(1 << ETH_GPIO_PIN(RMII_MII_RXD1)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_MII_RXD1)) | GPIO_MODE_OUT_PUSH_PULL
		);
/* RMII_MII_RXER */
  gpio_setup_mode(ETH_GPIO_PORT(RMII_MII_RXER),
			(1 << ETH_GPIO_PIN(RMII_MII_RXER)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_MII_RXER)) | GPIO_MODE_OUT_PUSH_PULL
		);
/* RMII_MII_TX_EN */
  gpio_setup_mode(ETH_GPIO_PORT(RMII_MII_TX_EN),
			(1 << ETH_GPIO_PIN(RMII_MII_TX_EN)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_MII_TX_EN)) | GPIO_MODE_OUT_PUSH_PULL
		);
/* RMII_MII_TXD0 */
  gpio_setup_mode(ETH_GPIO_PORT(RMII_MII_TXD0),
			(1 << ETH_GPIO_PIN(RMII_MII_TXD0)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_MII_TXD0)) | GPIO_MODE_OUT_PUSH_PULL
		);
/* RMII_MII_TXD1 */
  gpio_setup_mode(ETH_GPIO_PORT(RMII_MII_TXD1),
			(1 << ETH_GPIO_PIN(RMII_MII_TXD1)),
			GPIO_MODE_ALT_SET(ETH_GPIO_AF(RMII_MII_TXD1)) | GPIO_MODE_OUT_PUSH_PULL
		);

  /* Enable ETHERNET clock  */
  CONF_ETH_CLK_DEF_ETH();
#endif
  /* Enable the Ethernet global Interrupt */
  //HAL_NVIC_SetPriority(ETH_IRQn, 0x7, 0);
  //HAL_NVIC_EnableIRQ(ETH_IRQn);

  /* Enable ETHERNET clock  */
  //__HAL_RCC_ETH_CLK_ENABLE();
}
