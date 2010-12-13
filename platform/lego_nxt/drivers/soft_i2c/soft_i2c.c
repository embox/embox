/**
 * @file
 * @brief Soft I2C
 *
 * @date 12.12.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <drivers/pins.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <drivers/soft_i2c.h>
#include <drivers/at91sam7s256.h>
#include <drivers/at91_timer_counter.h>
#include <hal/reg.h>

#define TIMER 0
#define I2C_CLOCK 9600

EMBOX_UNIT_INIT(i2c_unit_init);

#define MAX_PORTS (sizeof(int))
static i2c_port_t *ports[MAX_PORTS];
static int port_count = 0;

static inline void pin_set_high(pin_mask_t mask) {
	pin_config_input(mask);
}

static inline void pin_set_low(pin_mask_t mask) {
	pin_config_output(mask);
	pin_clear_output(mask);
}

static void i2c_port_process(i2c_port_t *port) {
	switch (port->state) {
		case START:
			pin_set_low(port->sda);
			port->state = WRITE_FALL;
			break;
		case READ_FALL:
			if (pin_get_input(port->scl)) {
				pin_set_low(port->scl);
				pin_set_high(port->sda);
				port->state = READ_RISE;
				if (port->bit_cnt == 8) {
					port->data++;
					port->bit_cnt = 0;
					port->state = READ_ACK_RISE;
					if (port->data_cnt-- == 0) {
						port->state = STOP_DATA_FALL;
					}
				}
			}
			break;
		case READ_RISE:
			pin_set_high(port->scl);
			*(port->data) <<= 1;
			if (pin_get_input(port->sda)) {
				*(port->data) |= 1;
			}
			port->bit_cnt ++;
			port->state = READ_FALL;
			break;
		case READ_ACK_FALL:
			if (pin_get_input(port->scl)) {
				pin_set_low(port->scl);
				pin_set_high(port->sda);
				port->state = READ_RISE;
			}
			break;
		case READ_ACK_RISE:
			pin_set_low(port->sda);
			pin_set_high(port->scl);
			port->state = READ_ACK_FALL;
			break;
		case STOP_DATA_FALL:
			pin_set_low(port->sda);
			pin_set_high(port->scl);
			port->state = STOP;
			break;
		case STOP:
			if (pin_get_input(port->scl)) {
				pin_set_high(port->sda);
				port->state = IDLE;
			}
			break;
		case WRITE_FALL:
			if (pin_get_input(port->scl)) {
				pin_set_low(port->scl);
				if (port->bit_cnt == 0) {
					port->state = WRITE_ACK_RISE;
					if (port->data_cnt == 0) {
						port->state = STOP_DATA_FALL;
					}
				} else {
					port->state = WRITE_RISE;
				}
			}
			break;
		case WRITE_RISE:
			if (port->bit_cnt == 0) {
				port->bit_cnt = 8;
				port->data_cnt--;
				port->write_byte = *(port->data++);
			}
			if (port->write_byte & 0x80) {
				pin_set_high(port->sda);
			} else {
				pin_set_low(port->sda);
			}
			port->write_byte <<= 1;
			port->bit_cnt--;
			pin_set_high(port->scl);
			port->state = WRITE_FALL;
			break;
		case WRITE_ACK_RISE:
			pin_set_high(port->sda | port->scl);
			port->state = WRITE_ACK_FALL;
			break;
		case WRITE_ACK_FALL:
			if (pin_get_input(port->scl)) {
				pin_set_low(port->scl);
				if (pin_get_input(port->sda)) {
					port->state = STOP_DATA_FALL;
				} else {
					if (port->operation == WRITE) {
						port->state = WRITE_RISE;
					} else {
						port->state = READ_RISE;
					}
				}
			}
			break;
		case IDLE:
		default:
			break;
	}
}


static irq_return_t timer_handler(irq_nr_t irq_nr, void *data) {
	int i;
	 REG_LOAD(((uint8_t *) AT91C_TC0_SR) + TIMER * sizeof(AT91S_TCB));
	for (i = 0; i < port_count; i++) {
		i2c_port_process(ports[i]);
	}
	tc_reset(TIMER);
	return IRQ_HANDLED;
}

static void i2c_init(i2c_port_t *port) {
	ports[port_count++] = port;
}

static int i2c_unit_init(void) {
	tc_init(TIMER);
	tc_config_input(TIMER, (uint32_t) AT91C_TC_CLKS_TIMER_DIV1_CLOCK); /* MCLK/2, RC compare trigger */
	tc_set_limit(TIMER, (CONFIG_SYS_CLOCK/2)/(2 * I2C_CLOCK)); /* running on twice speed i2c_clock*/
	tc_limit_int_enable(TIMER, timer_handler);
	tc_reset(TIMER);
	return 0;
}

