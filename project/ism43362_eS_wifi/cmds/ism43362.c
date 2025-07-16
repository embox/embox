/**
 * @file ism43362.c
 * @brief ISM43362-M3G-L44-SPI Inventek eS-WiFi module library
 * @author Andrew Bursian
 * @version
 * @date 15.06.2023
 */
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <drivers/gpio.h>
#include <drivers/spi.h>

#include "ism43362.h"

#define WIFI_SPI_BUS	3
#define WIFI_CHIP_SELECT()	do {gpio_set(GPIO_PORT_E, 1<<0, GPIO_PIN_LOW);} while(0)
#define WIFI_CHIP_DESELECT()	do {gpio_set(GPIO_PORT_E, 1<<0, GPIO_PIN_HIGH);} while(0)
#define WIFI_IS_CMDDATA_READY()	(gpio_get(GPIO_PORT_E, 1<<1) != 0)

static struct spi_device *spi_dev;

static inline int gpio_setup_out_mode(unsigned short port, gpio_mask_t pins, int mode, int pinlevel) {
	int ret = gpio_setup_mode(port, pins, mode|GPIO_MODE_OUT);
	gpio_set(port, pins, pinlevel);
	return ret;
}

#include "stm32l475e_iot01.h"
static int SPI3_HAL_setup(void) {
	// HAL based spi configuration

/*	__HAL_RCC_SPI3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
*/
	/* configure SPI pins */
	GPIO_InitTypeDef GPIO_Init;
	memset(&GPIO_Init, 0, sizeof(GPIO_Init));

	/* configure SPI soft NSS pin for WiFi module */
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
	GPIO_Init.Pin       =  GPIO_PIN_0;
	GPIO_Init.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_Init.Pull      = GPIO_NOPULL;
	GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOE, &GPIO_Init);

	/* configure SPI CLK pin */
	GPIO_Init.Pin       =  GPIO_PIN_10;
	GPIO_Init.Mode      = GPIO_MODE_AF_PP;
	GPIO_Init.Pull      = GPIO_NOPULL;
	GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Init.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOC, &GPIO_Init);

	/* configure SPI MISO pin */
	GPIO_Init.Pin       = GPIO_PIN_11;
	GPIO_Init.Mode      = GPIO_MODE_AF_PP;
	GPIO_Init.Pull      = GPIO_PULLUP;
	GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Init.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOC,&GPIO_Init);

	/* configure SPI MOSI pin */
	GPIO_Init.Pin       = GPIO_PIN_12;
	GPIO_Init.Mode      = GPIO_MODE_AF_PP;
	GPIO_Init.Pull      = GPIO_NOPULL;
	GPIO_Init.Speed     = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_Init.Alternate = GPIO_AF6_SPI3;
	HAL_GPIO_Init(GPIOC, &GPIO_Init);

	/* SPI hardware init */
/*	SPI_HandleTypeDef hspi;
	memset(&hspi, 0, sizeof(hspi));
	hspi.Instance = SPI3;
	hspi.Init.Mode              = SPI_MODE_MASTER;
	hspi.Init.Direction         = SPI_DIRECTION_2LINES;
	hspi.Init.DataSize          = SPI_DATASIZE_16BIT;
	hspi.Init.CLKPolarity       = SPI_POLARITY_LOW;
	hspi.Init.CLKPhase          = SPI_PHASE_1EDGE;
	hspi.Init.NSS               = SPI_NSS_SOFT;
	hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // 80/8= 10MHz (Inventek WIFI module supports up to 20MHz)
	hspi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	hspi.Init.TIMode            = SPI_TIMODE_DISABLE;
	hspi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	hspi.Init.CRCPolynomial     = 0;

	for (int res = HAL_SPI_Init(&hspi); res != HAL_OK;) return res;
*/
	return 0;
}

int ism43362_init() {
	// WiFi module init (WakeUp, DRdy, Rst, yellow LED)
	gpio_setup_out_mode(GPIO_PORT_B, 1<<13, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_LOW);	// Wake up pin
	gpio_setup_mode(GPIO_PORT_E, 1<<1, GPIO_MODE_IN);				// Data ready pin
	gpio_setup_out_mode(GPIO_PORT_E, 1<<8, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_HIGH);	// Reset pin
	gpio_setup_out_mode(GPIO_PORT_C, 1<<9, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_HIGH); // WiFi LED pin

	// SPI3 HAL setup
	for (int res = SPI3_HAL_setup(); res;) {return -10500;}

	// Get SPI device pointer
	if (!(spi_dev = spi_dev_by_id(WIFI_SPI_BUS))){
		return -10006;
	}

	// Reset WiFi module
	gpio_set(GPIO_PORT_E, 1<<8, GPIO_PIN_LOW);
	usleep(100000);
	gpio_set(GPIO_PORT_E, 1<<8, GPIO_PIN_HIGH);
	usleep(50000);

	// Wait for WiFi module to be ready for exchange
	// Wait for CMD/DATA_READY sign
	for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(5000)){
		if ((clock() - t0) > 10000){
			 return -10001;	// No CMD/DATA_READY sign after reset
		}
	}

	// Read initial prompt
	char pad[2]= {'\n','\n'}, in[2];
	WIFI_CHIP_SELECT();
	usleep(15); // datasheet requires 15 us
	
	for (int i = 0; WIFI_IS_CMDDATA_READY(); i++) {
		if (spi_transfer(spi_dev, (uint8_t*)pad, (uint8_t*)in, 1) < 0) {
			WIFI_CHIP_DESELECT(); 
			return -10002;
		} // SPI transfer error while reading initial prompt
		
		if (i > 5) {
			WIFI_CHIP_DESELECT(); 
			return -10003;
		} // Too many characters while reading initial prompt
	}
	WIFI_CHIP_DESELECT();

	if ((in[0] != '>') || (in[1] != ' '))
		return -10004; // Initial prompt not valid
		
	for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(1000)){
		if ((clock() - t0) > 100){
			return -10005;	// No CMD/DATA_READY sign after initial prompt
		}
	}

	return 0;
}

/*	Low level exchange action with module
	only trailling NAKs are excluded in answer
	returns received bytes count or negative error code. 
		In particular, if rxl is not enough for full answer, return value is -count, means you have to continue with txb=NULL.
		Other error codes are less than 10000
        txb - transfer string pointer,
        txl - transfer string length
        rxb - receive buffer, it must be greater than txb (it used as trash while sending command)
        	good size for rxb is about char[2048]
        rxl - its length
	if txb==NULL or txl==0 only reading performed
	if txl<=0 strlen(txb) used and ending \r added if nesessary
*/

int ism43362_exchange(char *txb, int txl, char *rxb, int rxl) {
	char tail[2]={'\r','\n'};
	char pads[2]={'\n','\n'};
	char in[2];
	int rxc=0;

	// Prepare command
	if (txb && txl <= 0) {
		txl=strlen(txb);
		if (txl && txb[txl-1] != '\r')
			txb[txl++] = 'r';	// ???? Change txb!
	}

	if (txb && txl) {
		// Ensure module readiness
		for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(1000)){
			if ((clock() - t0) > 100){
				return -10006;		// No CMD/DATA_READY sign for more than 100 ms
			}
		}

		// Send command
		WIFI_CHIP_SELECT();
		if (spi_transfer(spi_dev, (uint8_t*)txb, (uint8_t*)rxb, txl/2) < 0){
			WIFI_CHIP_DESELECT(); 
			return -10007;
		}		// SPI transfer error while sending command
		
		if (txl & 1){
			if (spi_transfer(spi_dev, (uint8_t*)tail, (uint8_t*)in, 1) < 0){
				WIFI_CHIP_DESELECT();
				return -10007;
				}	// SPI transfer error while sending command
		}

		WIFI_CHIP_DESELECT();
		/* Below is danger cycle because it can miss very fast CMD/DATA READY = 0 signal */
		/* maybe it would be better to use usleep(xxx), but the value of xxx is not defined by Inventek */
		for (clock_t t0 = clock(); WIFI_IS_CMDDATA_READY();){
			if ((clock() - t0) > 10) {
				return -10008;				// Error waiting CMD/DATA READY sign disappears
			}
		}
	}

	// Ensure module readiness
	for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(10000)) if ((clock() - t0) > 10000) return -10009;			// No CMD/DATA_READY sign (no answer) for more than 10 s

	// Get an answer
	WIFI_CHIP_SELECT();
	usleep(15);	// datasheet requires 15 us

	for (clock_t t0 = clock(); WIFI_IS_CMDDATA_READY();) {
		if (rxc > rxl-2){
			WIFI_CHIP_DESELECT(); 
			return -rxc;
		}	// rxb is too small for full answer
		
		if ((clock() - t0) > 1000){
			WIFI_CHIP_DESELECT(); 
			return -10010;
		}	// No end of answer for more than 1 s	
		
		if (spi_transfer(spi_dev, (uint8_t*)pads, (uint8_t*)in, 1) < 0){
			WIFI_CHIP_DESELECT(); 
			return -10011;
		}			// SPI transfer error while reading answer
		rxb[rxc++] = in[0];
		rxb[rxc++] = in[1];
	}
	WIFI_CHIP_DESELECT();
	
	if (rxc && rxb[rxc-1] == 0x15) rxc-=1;
	if (rxc && rxb[rxc-1] == 0x15) rxc-=1;
	if (rxc && rxb[rxc-1] == 0x15) rxc-=1;

	return rxc;
}
/*	Calls ism43362_exchange and parse the answer
	returns received payload bytes count (may be 0) or negative error code.
		In particular, if rxb is not enough for full answer, return value is -payload count, means you have to continue with txb=NULL.
		???? The problem is the ending "\r\nOK\r\n> ", so if 0 < payload count < -10 (in result of ism43362_interact(NULL, ...)), it means you have to cut previous result
	payload starts at rxb[0]
	if there was an asinchronous message inside module answer, 
	    it stored for mr command, also flag "ASINC_MESSAGE" is raised
	???if rxl < 0 all special symbols except \r, \n, and \t substained with "\\x%02x" 4-character format
	if rxb is not enough for full answer, return value is -payload count, means you have to continue with txb=NULL..
	the problem is the ending "\r\nOK\r\n> ", so if 0 < payload count < -10 (in result of ism43362_interact(NULL, ...)), 
		it means you have to cut previous result
*/
int ism43362_interact(char *txb, int txc, char *rxb, int rxl) {
	return -500; // Stub
}

