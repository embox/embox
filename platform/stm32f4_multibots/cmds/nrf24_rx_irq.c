
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <embox/unit.h>

#include "stm32f4_discovery.h"

#include "nrf24.h"

#include <kernel/irq.h>
#include <kernel/printk.h>

extern int spi_init(void);

static int EXTILine0_Config(void);

static void spi_delay(int n) {
	int i = n;
	while (i--)
		;
}

extern uint8_t spi_transfer(uint8_t tx);

static void nrf24_test(void) {
	uint8_t rx_address[5] = {0xA7,0x95,0xF1,0x36,0x07};
	uint8_t tx_address[5] = {0xA7,0x95,0xF1,0x36,0x06};
	//uint8_t tx_address[5] = {0x17,0x97,0xA7,0xA7,0xD7};
	uint8_t addr[10] = {0x1};
	uint8_t val = 0x17;
	uint8_t reg;

	spi_delay(1000000);
	/* init hardware pins */
	nrf24_init();

	if (spi_init() < 0) {
		printf("ERRRRROR SPI!\n");
	}

	/* Returned value is not correct on the first read, who knows why... */
	nrf24_readRegister(CONFIG, &reg, 1);

	nrf24_readRegister(CONFIG, &reg, 1);
	printf("CONFIG (before configured) = %x\n", reg);
	nrf24_readRegister(EN_AA, &reg, 1);
	printf("EN_AA = %x\n", reg);
	nrf24_readRegister(RF_SETUP, &reg, 1);
	printf("RF_SETUP = %x\n", reg);
	nrf24_readRegister(SETUP_AW, &reg, 1);
	printf("SETUP_AW = %x\n", reg);
	nrf24_readRegister(SETUP_RETR, &reg, 1);
	printf("SETUP_RETR = %x\n", reg);
	nrf24_readRegister(FIFO_STATUS, &reg, 1);
	printf("FIFO_STATUS = %x\n", reg);

	nrf24_readRegister(RF_CH, &reg, 1);
	printf("RF_CH = %x\n", reg);
	nrf24_writeRegister(RF_CH, &val, 1);
	nrf24_readRegister(RF_CH, &reg, 1);
	printf("RF_CH = %x\n", reg);
	val = 2;
	nrf24_writeRegister(RF_CH, &val, 1);
	nrf24_readRegister(RF_CH, &reg, 1);
	printf("RF_CH = %x\n", reg);

#if 1
	nrf24_readRegister(RX_ADDR_P0, addr, 5);
	printf("RX_ADDR_P0: %x %x %x %x %x\n", addr[0], addr[1], addr[2], addr[3], addr[4]);
	nrf24_writeRegister(RX_ADDR_P0, tx_address,5);
	spi_delay(1000000);
	nrf24_readRegister(RX_ADDR_P0, addr, 5);
	printf("RX_ADDR_P0: %x %x %x %x %x\n", addr[0], addr[1], addr[2], addr[3], addr[4]);
	
	spi_delay(1000000);
	nrf24_readRegister(RF_CH, &reg, 1);

	nrf24_readRegister(TX_ADDR, addr, 5);
	printf("TX_ADDR: %x %x %x %x %x\n", addr[0], addr[1], addr[2], addr[3], addr[4]);
	nrf24_writeRegister(TX_ADDR, tx_address,5);
	nrf24_readRegister(TX_ADDR, addr, 5);
	printf("TX_ADDR: %x %x %x %x %x\n", addr[0], addr[1], addr[2], addr[3], addr[4]);

	spi_delay(1000000);

	nrf24_readRegister(RX_ADDR_P1, addr, 5);
	printf("RX_ADDR_P1: %x %x %x %x %x\n", addr[0], addr[1], addr[2], addr[3], addr[4]);
	nrf24_writeRegister(RX_ADDR_P1, rx_address,5);
	nrf24_readRegister(RX_ADDR_P1, addr, 5);
	printf("RX_ADDR_P1: %x %x %x %x %x\n", addr[0], addr[1], addr[2], addr[3], addr[4]);
#endif

	/* Channel #2 , payload length: 4 */
	nrf24_config(16,6);

	if (EXTILine0_Config() < 0) {
		printf("EXTILine0_Config error!!!!!\n");
	}

    nrf24_readRegister(FIFO_STATUS,&reg,1);
	printf("!> 1 FIFO_STATUS = %2X\n", reg);

	spi_delay(1000000);

	nrf24_readRegister(CONFIG, &reg, 1);
	nrf24_readRegister(CONFIG, &reg, 1);
	printf("CONFIG (after configured) = %x\n", reg);
	nrf24_readRegister(EN_AA, &reg, 1);
	printf("EN_AA = %x\n", reg);
	nrf24_readRegister(RF_SETUP, &reg, 1);
	printf("RF_SETUP = %x\n", reg);

	nrf24_readRegister(TX_ADDR, addr, 5);
	printf("TX addr is: %x %x %x %x %x\n", addr[0], addr[1], addr[2], addr[3], addr[4]);
	//spi_delay(1000000);
	nrf24_readRegister(RX_ADDR_P0, addr, 5);
	printf("RX addr is: %x %x %x %x %x\n", addr[0], addr[1], addr[2], addr[3], addr[4]);
	//spi_delay(1000000);
	nrf24_readRegister(RX_ADDR_P1, addr, 5);
	printf("RX_ADDR_P1 addr is: %x %x %x %x %x\n", addr[0], addr[1], addr[2], addr[3], addr[4]);
    
	nrf24_readRegister(FIFO_STATUS,&reg,1);
	printf("!!!!!!> 2 FIFO_STATUS = %2X\n", reg);

	nrf24_readRegister(FIFO_STATUS,&reg,1);
	printf("!!!!!!> 3 FIFO_STATUS = %2X\n", reg);

	while (1)
		;
}

static irq_return_t exti0_handler(unsigned int irq_nr, void *data) {
	uint8_t status;
	uint8_t data_array[6];

	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	}

	status = nrf24_getStatus();

	nrf24_getData(data_array);
	printf("> ");
	printf("%2X ",data_array[0]);
	printf("%2X ",data_array[1]);
	printf("%2X ",data_array[2]);
	printf("%2X ",data_array[3]);
	printf("%2X ",data_array[4]);
	printf("%2X\r\n",data_array[5]);

	if (status & (1 << RX_DR)) {
		nrf24_configRegister(STATUS,(1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT));
	} else if (status & (1 << TX_DS)) {
		nrf24_configRegister(STATUS,(1<<TX_DS));
	} else if (status & (1 << MAX_RT)) {
		nrf24_configRegister(STATUS,(1<<MAX_RT));
	} else {
		printk("(irq) Unknown interrupt\n");
	}

	return IRQ_HANDLED;
}

static int EXTILine0_Config(void) {
	GPIO_InitTypeDef   GPIO_InitStructure;

	printf(">>>6 EXTILine0_Config\n");

	/* Enable GPIOA clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Configure PA0 pin as input floating */
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin  = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	return irq_attach(EXTI0_IRQn + 16, exti0_handler, 0, NULL, "stm32 systick timer");
}

static void init_leds() {
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
}

int main(int argc, char *argv[]) {
	printf("NRF24 recv test start\n");

	init_leds();
	BSP_LED_Toggle(LED5);

	nrf24_test();

	return 0;
}
