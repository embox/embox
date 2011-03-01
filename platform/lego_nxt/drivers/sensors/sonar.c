/**
 * @file
 * @brief nxt sonar driver
 *
 * @date 17.01.11
 * @author Alexander Batyukov
 */

#include <types.h>
#include <embox/test.h>
#include <unistd.h>
#include <drivers/soft_i2c.h>

#include <drivers/nxt_touch_sensor.h>

int sonar_sensor_get_val(sensor_t *sensor) {
	i2c_port_t port;
	uint8_t sonar_val;
	uint8_t reg = 0x42;

	port = sensor->i2c_port;
	i2c_write(&port, 1, &reg, 1);
	while (port.state != IDLE) {
	}
	i2c_read(&port, 1, &sonar_val, 1);
	while (port.state != IDLE) {
	}
	printf("res = %d\n", sonar_val);
	return sonar_val;
}

void sonar_sensor_init (sensor_t *sensor) {
	i2c_port_t port;

	port = sensor->i2c_port;
	i2c_init(&port);
	return;
}

