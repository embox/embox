/**
 * @file
 * @brief
 *
 * @date    09.04.2026
 * @author  Efim Perevalov
 */
#include <unistd.h>
#include <util/log.h>

#include "bmp280.h"
#include "bmp280_transfer.h"

/* https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf*/

#define BMP280_ID            0xD0
#define BMP280_RESET         0xE0
#define BMP280_STATUS        0xF3
#define BMP280_CTRL_MEAS     0xF4
#define BMP280_CONFIG        0xF5
#define BMP280_PRESS_MSB     0xF7
#define BMP280_PRESS_LSB     0xF8
#define BMP280_PRESS_XLSB    0xF9
#define BMP280_TEMP_MSB      0xFA
#define BMP280_TEMP_LSB      0xFB
#define BMP280_TEMP_XLSB     0xFC

#define BMP280_CALIB_START   0x88
#define BMP280_CHIP_ID_VAL   0x58
#define BMP280_RESET_VAL     0xB6

extern struct bmp280_dev bmp280_dev0;

int16_t bmp280_get_temperature(void) {
	// recommended to use a burst read and not address every register individually
	uint8_t msb, lsb, xlsb;
	
    bmp280_readb(&bmp280_dev0, BMP280_TEMP_MSB,  &msb);
    bmp280_readb(&bmp280_dev0, BMP280_TEMP_LSB,  &lsb);
    bmp280_readb(&bmp280_dev0, BMP280_TEMP_XLSB, &xlsb);

    return ((int32_t)msb << 12) | ((int32_t)lsb << 4) | (xlsb >> 4);
}

int16_t bmp280_get_humidity(void) {

	return 0;
}

int16_t bmp280_get_pressure(void) {

	return 0;
}

int bmp280_init(void) {
    uint8_t chip_id;

    bmp280_readb(&bmp280_dev0, BMP280_ID, &chip_id);
    if (chip_id != BMP280_CHIP_ID_VAL) {
        log_error("BMP280: wrong chip id 0x%02x\n", chip_id);
        return -1;
    }

    return 0;
}