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

#include <drivers/gpio/gpio.h>
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

int ism43362_init() {
	// WiFi module init (WakeUp, DRdy, Rst, yellow LED)
	gpio_setup_out_mode(GPIO_PORT_B, 1<<13, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_LOW);	// Wake up pin
	gpio_setup_mode(GPIO_PORT_E, 1<<1, GPIO_MODE_IN);				// Data ready pin
	gpio_setup_out_mode(GPIO_PORT_E, 1<<8, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_HIGH);	// Reset pin
	gpio_setup_out_mode(GPIO_PORT_C, 1<<9, GPIO_MODE_OUT_PUSH_PULL, GPIO_PIN_HIGH); // WiFi LED pin

	// Get SPI device pointer
	spi_dev = spi_dev_by_id(WIFI_SPI_BUS);

	// Reset WiFi module
	gpio_set(GPIO_PORT_E, 1<<8, GPIO_PIN_LOW);
	usleep(100000);
	gpio_set(GPIO_PORT_E, 1<<8, GPIO_PIN_HIGH);
	usleep(50000);

	// Wait for WiFi module to be ready for exchange
	// Wait for CMD/DATA_READY sign
	for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(5000)) if ((clock() - t0) > 10000) return -1;	// No CMD/DATA_READY sign after reset

	// Read initial prompt
	char pad[2]={'\n','\n'}, in[2];
	WIFI_CHIP_SELECT();
	usleep(15); // datasheet requires 15 us
	for (int i = 0; WIFI_IS_CMDDATA_READY(); i++) {
		if (spi_transfer(spi_dev, (uint8_t*)pad, (uint8_t*)in, 1) < 0) {WIFI_CHIP_DESELECT(); return -2;}	// SPI transfer error while reading initial prompt
		if (i > 5) {WIFI_CHIP_DESELECT(); return -3;}								// Too many characters while reading initial prompt
	}
	WIFI_CHIP_DESELECT();

	if ((in[0] != '>') || (in[1] != ' ')) return -4;								// Initial prompt not valid
		
	for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(1000)) if ((clock() - t0) > 100) return -5;		// No CMD/DATA_READY sign after initial prompt

	return 0;
}

/*	Low level exchange action with module
	only trailling NAKs are excluded in answer
	returns received bytes count or negative error code
        txb - transfer string pointer,
        txl - transfer string length
        rxb - receive buffer, it must be greater than txb (it used as trash while sending command)
        	good size for rxb is about char[2048]
        rxl - its length
	if txb==NULL or txl==0 only reading performed
	if txc<=0 strlen(txb) used and ending \r added if nesessary
	if rxl is not enough for full answer, return value is -1000000-count, means you have to continue with txb=NULL..
*/

int ism43362_exchange(char *txb, int txl, char *rxb, int rxl) {
	char tail[2]={'\r','\n'};
	char pads[2]={'\n','\n'};
	char in[2];
	int rxc=0;

	// Prepare command
	if (txb && txl <= 0) {
		txl=strlen(txb);
		if (txl && txb[txl-1] != '\r') txb[txl++] = 'r';	// ???? Change txb!
	}

	if (txb && txl) {
		// Ensure module readiness
		for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(1000)) if ((clock() - t0) > 100) return -6;		// No CMD/DATA_READY sign for more than 100 ms
		
		// Send command
		WIFI_CHIP_SELECT();
		if (spi_transfer(spi_dev, (uint8_t*)txb, (uint8_t*)rxb, txl/2) < 0) {WIFI_CHIP_DESELECT(); return -7;}		// SPI transfer error while sending command
		if (txl & 1) if (spi_transfer(spi_dev, (uint8_t*)tail, (uint8_t*)in, 1) < 0) {WIFI_CHIP_DESELECT(); return -7;}	// SPI transfer error while sending command
		WIFI_CHIP_DESELECT();
		/* Below is danger cycle because it can miss very fast CMD/DATA READY = 0 signal */
		/* maybe it would be better to use usleep(xxx), but the value of xxx is not defined by Inventek */
		for (clock_t t0 = clock(); WIFI_IS_CMDDATA_READY();) if ((clock() - t0) > 10) return -8;			// Error waiting CMD/DATA READY sign disappears
	}

	// Ensure module readiness
	for (clock_t t0 = clock(); !WIFI_IS_CMDDATA_READY(); usleep(10000)) if ((clock() - t0) > 10000) return -9;		// No CMD/DATA_READY sign (no answer) for more than 10 s

	// Get an answer
	WIFI_CHIP_SELECT();
	usleep(15);	// datasheet requires 15 us

	for (clock_t t0 = clock(); WIFI_IS_CMDDATA_READY();) {
		if (rxc > rxl-2) {WIFI_CHIP_DESELECT(); return -1000000-rxc;}							// rxb is too small for full answer
		if ((clock() - t0) > 1000) {WIFI_CHIP_DESELECT(); return -10;}							// No end of answer for more than 1 s	
		if (spi_transfer(spi_dev, (uint8_t*)pads, (uint8_t*)in, 1) < 0) {WIFI_CHIP_DESELECT(); return -11;}		// SPI transfer error while reading answer
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
	returns received payload bytes count (may be 0) or negative error code
	payload starts at rxb[0]
	if there was an asinchronous message inside module answer, 
	    it stored for mr command, also flag "ASINC_MESSAGE" is raised
	???if rxl < 0 all special symbols except \r, \n, and \t substained with "\\x%02x" 4-character format
	if rxb is not enough for full answer, return value is -1000-payload count, means you have to continue with txb=NULL..
	the problem is the ending "\r\nOK\r\n> " 
*/
int ism43362_interact(char *txb, int txc, char *rxb, int rxl) {
	return -500; // Stub
}

