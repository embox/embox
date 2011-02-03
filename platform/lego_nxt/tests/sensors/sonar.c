/**
 * @file
 * @brief Sonar test
 *
 * @date 22.12.2010
 * @author Anton Kozlov
 */

#include <drivers/soft_i2c.h>
#include <drivers/nxt_buttons.h>
#include <drivers/nxt_sensor.h>
#include <embox/test.h>
#include <drivers/at91sam7s256.h>
#include <unistd.h>

EMBOX_TEST(sonar_test);

#define PORT ((i2c_port_t *) (&sensors[1].i2c_port))

static uint8_t sonar_val = 0x34;
static uint8_t reg = 0x42;

static void read_val(i2c_port_t *port) {
	i2c_write(PORT, 1, &reg, 1);
	while (port->state != IDLE) {
	}
	i2c_read(PORT, 1, &sonar_val, 1);
	while (port->state != IDLE) {
	}
	TRACE("\n0x%x",  sonar_val);
}

static int sonar_test(void) {
	int buts;
	i2c_init(PORT);

	while (!(( buts = nxt_buttons_was_pressed()) & BT_DOWN)) {
		read_val((i2c_port_t *) PORT);
		usleep(1000);
	}
	return 0;
}

