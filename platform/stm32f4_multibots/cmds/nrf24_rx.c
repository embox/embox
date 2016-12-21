
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <embox/unit.h>

#include "stm32f4_discovery.h"

#include "nrf24.h"

extern int spi_init(void);

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
	uint8_t data_array[6];
	uint32_t counter = 0;

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

	/* Set the device addresses */
	//nrf24_tx_address(rx_address);
	//nrf24_rx_address(rx_address);

	//nrf24_ce_digitalWrite(HIGH);

	//reg = nrf24_getStatus();
	//printf("STATUS = %2X\n", reg);

	nrf24_readRegister(FIFO_STATUS,&reg,1);
	printf("!!!!!!> 3 FIFO_STATUS = %2X\n", reg);

	while(1) {
		if(nrf24_dataReady())
		{
			counter = 0;
			//reg = nrf24_getStatus();
			//printf("STATUS = %x\n", reg);
			nrf24_getData(data_array);
			printf("> ");
			printf("%2X ",data_array[0]);
			printf("%2X ",data_array[1]);
			printf("%2X ",data_array[2]);
			printf("%2X ",data_array[3]);
			printf("%2X ",data_array[4]);
			printf("%2X\r\n",data_array[5]);
			//spi_delay(1000);
		} else if (counter > 100000) {
			reg = nrf24_getStatus();
			printf("(counter > 100000) STATUS = %2X\n", reg);
			nrf24_readRegister(FIFO_STATUS,&reg,1);
			printf("(counter > 100000) FIFO_STATUS = %2X\n", reg);
		}
		counter++;
	}
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
