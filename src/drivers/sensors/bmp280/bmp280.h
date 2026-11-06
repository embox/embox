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

struct bmp280_snapshot {
	uint8_t id;
	uint8_t status;
	uint8_t ctrl_meas;
	uint8_t config;
	uint8_t burst[6];
	uint8_t single[6];
};

extern int bmp280_init(void);
/* Temperature x1; pressure_osrs: 0=off, 1=x1, 2=x2, ... 5=x16.
 * Wait for the single conversion to finish and leave the sensor in sleep.
 */
extern int bmp280_force_measurement(unsigned int pressure_osrs);
/* Read registers without compensation, including when ADC data is invalid. */
extern int bmp280_read_snapshot(struct bmp280_snapshot *snapshot);
/* Temperature: 0.01 degrees Celsius; pressure: Pa. Returns negative errno. */
extern int bmp280_read_values(int32_t *temperature, int32_t *pressure);
/* Legacy getters return INT32_MIN on error, in the units above. */
extern int32_t bmp280_get_temperature(void);
extern int32_t bmp280_get_humidity(void);
extern int32_t bmp280_get_pressure(void);

#endif /* BMP280_H_ */
