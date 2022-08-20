/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */
#include <libactuators/motor.h>

#include <drivers/gpio/gpio.h>

static void stm32f3_delay(uint32_t delay) {
	while(delay--)
		;
}

void motor_init(struct motor *m) {

	m->port = GPIO_PORT_D;
	m->enable = MOTOR_ENABLE1;
	m->input[0] = MOTOR_INPUT1;
	m->input[1] = MOTOR_INPUT2;

	gpio_setup_mode(m->port, m->enable | m->input[0] | m->input[1], GPIO_MODE_OUTPUT);
}

void motor_enable(struct motor *m) {
	gpio_set(m->port, m->enable, GPIO_PIN_HIGH);
}


void motor_disable(struct motor *m) {
	gpio_set(m->port, m->enable, GPIO_PIN_LOW);
}


void motor_run(struct motor *m, enum motor_run_direction dir) {
	uint8_t input_pin;

	input_pin = (dir == MOTOR_RUN_FORWARD) ? 1 : 0;

	gpio_set(m->port, m->input[!input_pin], GPIO_PIN_LOW);
	stm32f3_delay(0xFF); /* FIXME may be it is not needed */
	gpio_set(m->port, m->input[input_pin], GPIO_PIN_HIGH);
}

void motor_stop(struct motor *m) {
	stm32f3_delay(1000);

	motor_disable(m);
	stm32f3_delay(1000);
	gpio_set(m->port, m->input[0], GPIO_PIN_LOW);
	stm32f3_delay(1000);
	gpio_set(m->port, m->input[1], GPIO_PIN_LOW);
	stm32f3_delay(1000);
}
