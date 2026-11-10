#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <util/log.h>

#include "spl06.h"
#include "spl06_transfer.h"

#define SPL06_ID            0x0D
#define SPL06_RESET         0x0C
#define SPL06_STATUS        0x0A
#define SPL06_CTRL_MEAS     0x08
#define SPL06_CONFIG        0x09
#define SPL06_PRESS_MSB     0x00
#define SPL06_PRESS_LSB     0x01
#define SPL06_PRESS_XLSB    0x02
#define SPL06_TEMP_MSB      0x03
#define SPL06_TEMP_LSB      0x04
#define SPL06_TEMP_XLSB     0x05

#define SPL06_CHIP_ID_VAL   0x10
#define SPL06_RESET_VAL     0x00

#define SPL06_NORMAL_MODE   0x14

extern struct spl06_dev spl06_dev0;

int32_t spl06_get_temperature(void) {
	// recommended to use a burst read and not address every register individually
	uint8_t msb, lsb, xlsb;
	
    spl06_readb(&spl06_dev0, SPL06_TEMP_MSB,  &msb);
    spl06_readb(&spl06_dev0, SPL06_TEMP_LSB,  &lsb);
    spl06_readb(&spl06_dev0, SPL06_TEMP_XLSB, &xlsb);

    return ((int32_t)msb << 12) | ((int32_t)lsb << 4) | (xlsb >> 4);
}

int32_t spl06_get_humidity(void) {

	return 0;
}

int32_t spl06_get_pressure(void) {
    // recommended to use a burst read and not address every register individually
    uint8_t msb, lsb, xlsb;
	
    spl06_readb(&spl06_dev0, SPL06_PRESS_MSB,  &msb);
    spl06_readb(&spl06_dev0, SPL06_PRESS_LSB,  &lsb);
    spl06_readb(&spl06_dev0, SPL06_PRESS_XLSB, &xlsb);

    return ((int32_t)msb << 12) | ((int32_t)lsb << 4) | (xlsb >> 4);
}

int spl06_init(void) {
    uint8_t chip_id;

    spl06_readb(&spl06_dev0, SPL06_ID, &chip_id);
    if (chip_id != SPL06_CHIP_ID_VAL) {
        printf("SPL06: wrong chip id 0x%02x\n", chip_id);
        return -1;
    }else{
        printf("spl06 inited. ID: 0x%02x\n", chip_id);
    }

    return 0;
}
