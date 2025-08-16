/**
 * @file 
 * @brief ISM43362-M3G-L44-SPI Inventek eS-WiFi module library
 * @author Daria Pechenova
 * @version
 * @date 18.07.2025
 */

#include <time.h>
#include <string.h>
#include <unistd.h>

#include <drivers/gpio.h>
#include <drivers/spi.h>

#include <config/board_config.h>
#include <bsp/stm32cube_hal.h> /* FIXUP! only for STM PINS configuration */

#include <libs/ism43362.h>
#include "ism43362_config.h"


// #define WIFI_LED_PIN (1 << 9)

#define WIFI_CHIP_SELECT()	do {gpio_set(CONF_SPI_PIN_CS_PORT, CONF_SPI_PIN_CS_NR, GPIO_PIN_LOW);} while(0)
#define WIFI_CHIP_DESELECT()	do {gpio_set(CONF_SPI_PIN_CS_PORT, CONF_SPI_PIN_CS_NR, GPIO_PIN_HIGH);} while(0)
#define WIFI_IS_CMDDATA_READY()	(gpio_get(CONF_SPI_PIN_CMDDATA_PORT, CONF_SPI_PIN_CMDDATA_NR) != 0)


static struct spi_device *spi_dev;

static inline int gpio_setup_out_mode(unsigned short port, gpio_mask_t pins, int mode, int pinlevel) {
	int ret = gpio_setup_mode(port, pins, mode|GPIO_MODE_OUT);
	gpio_set(port, pins, pinlevel);
	return ret;
}

static inline int spi_pins_setup(void){

 	/* configure SPI soft NSS pin for WiFi module */
	gpio_setup_out_mode(CONF_SPI_PIN_CS_PORT, CONF_SPI_PIN_CS_NR, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_HIGH);
	
	/* configure SPI CLK pin */
	gpio_setup_mode(CONF_SPI_PIN_SCK_PORT, CONF_SPI_PIN_SCK_NR, GPIO_MODE_ALT_SET(CONF_SPI_PIN_SCK_AF) | GPIO_MODE_OUT_PUSH_PULL);
	
	/* configure SPI MISO pin */
	gpio_setup_mode(CONF_SPI_PIN_MISO_PORT, CONF_SPI_PIN_MISO_NR, GPIO_MODE_ALT_SET(CONF_SPI_PIN_MISO_AF) | GPIO_MODE_OUT_PUSH_PULL);

	/* configure SPI MOSI pin */
	gpio_setup_mode(CONF_SPI_PIN_MOSI_PORT, CONF_SPI_PIN_MOSI_NR, GPIO_MODE_ALT_SET(CONF_SPI_PIN_MOSI_AF) | GPIO_MODE_OUT_PUSH_PULL);

	return 0;
}

int ism43362_init() {
	// WiFi module init (WakeUp, DRdy, Rst, yellow LED)
	gpio_setup_out_mode(CONF_SPI_PIN_WAKE_PORT, CONF_SPI_PIN_WAKE_NR, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_LOW);	// Wake up pin
	gpio_setup_mode(CONF_SPI_PIN_CMDDATA_PORT, CONF_SPI_PIN_CMDDATA_NR, GPIO_MODE_IN);				// Data ready pin
	gpio_setup_out_mode(CONF_SPI_PIN_RESET_PORT, CONF_SPI_PIN_RESET_NR, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_HIGH);	// Reset pin
	gpio_setup_out_mode(CONF_SPI_PIN_WIFI_LED_PORT, CONF_SPI_PIN_WIFI_LED_NR, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_HIGH); // WiFi LED pin

	spi_pins_setup();

	// Get SPI device pointer
	if (!(spi_dev = spi_dev_by_id(WIFI_SPI_BUS))){
		return -10006;
	}

	// Reset WiFi module
	gpio_set(CONF_SPI_PIN_RESET_PORT, CONF_SPI_PIN_RESET_NR, GPIO_PIN_LOW);
	usleep(100000);
	gpio_set(CONF_SPI_PIN_RESET_PORT, CONF_SPI_PIN_RESET_NR, GPIO_PIN_HIGH);
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
		}

		if (i > 5) {
			WIFI_CHIP_DESELECT(); 
			return -10003;
		}
	}
	WIFI_CHIP_DESELECT();

	if ((in[0] != '>') || (in[1] != ' '))
		return -10004; // Initial prompt not valid
		
	for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(1000)){
		if ((clock() - t0) > 100){
			return -10005;	// No CMD/DATA_READY sign after initial prompt
		}
	}

	// gpio_set(GPIO_PORT_C, 1 << 9, GPIO_MODE_IN); // WiFi LED pin

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
	for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(40000)) {
		if ((clock() - t0) > 10000){
			return -10009;			// No CMD/DATA_READY sign (no answer) for more than 10 s
		}
	}

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
