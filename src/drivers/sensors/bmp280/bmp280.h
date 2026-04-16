#ifndef BMP280_TRANSFER_H_
#define BMP280_TRANSFER_H_

#include <stdint.h>

extern int bmp280_init(void);
extern int16_t bmp280_get_temperature(void);
extern int16_t bmp280_get_humidity(void);
extern int16_t bmp280_get_pressure(void);

#endif /* BMP280_TRANSFER_H_ */