/**
 * @file
 * @brief
 *
 * @date    09.04.2026
 * @author  Efim Perevalov
 */

#ifndef BMP280_H_
#define BMP280_H_

#include <stdint.h>

extern int bmp280_init(void);
extern int32_t bmp280_get_temperature(void);
extern int32_t bmp280_get_humidity(void);
extern int32_t bmp280_get_pressure(void);

#endif /* BMP280_H_ */