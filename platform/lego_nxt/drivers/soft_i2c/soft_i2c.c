/**
 * @file
 * @brief Soft I2C
 *
 * @date 12.12.10
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
i2c_port_t *ports[MAX_PORTS];
static int port_count = 0;

void pin_set_high(pin_mask_t mask) {
	pin_config_input(mask);
}

void pin_set_low(pin_mask_t mask) {
	pin_clear_output(mask);
	pin_config_output(mask);
}

void scl_set_low(pin_mask_t mask) {
	pin_clear_output(mask);
}

void scl_set_high(pin_mask_t mask) {
	pin_set_output(mask);
}

i2c_state_t wait_state;

static void i2c_port_process(i2c_port_t *port) {
	REG_LOAD(AT91C_TC0_SR); //XXX
	switch (port->state) {
	case START:
		if (pin_get_input(port->sda) && pin_get_input(port->scl)) {
			pin_set_low(port->sda);
			port->state = WRITE_FALL;
		}
		break;
	case READ_FALL:
		if (pin_get_input(port->scl)) {
			scl_set_low(port->scl);
			port->state = READ_RISE;
			if (port->bit_cnt == 8) {
				port->data++;
				port->bit_cnt = 0;
				port->state = READ_ACK_RISE;
				if (port->data_cnt-- == 0) {
					port->state = READ_NOT_ACK_RISE;
				}
			}
		}
		break;
	case READ_NOT_ACK_RISE:
		pin_set_high(port->sda);
		scl_set_high(port->scl);
		port->state = READ_NOT_ACK_FALL;
		break;
	case READ_NOT_ACK_FALL:
		if (pin_get_input(port->scl)) {
			scl_set_low(port->scl);
			port->state = STOP_DATA_FALL;
		}
		break;
	case READ_RISE:
		scl_set_high(port->scl);
		if (pin_get_input(port->scl)) {
			uint8_t *ptr;
			ptr = port->data;
			*ptr = *ptr << 1;
			if (pin_get_input(port->sda)) {
				*ptr = *ptr | 1;
			}
			port->bit_cnt++;
			port->state = READ_FALL;
		}
		break;
	case READ_ACK_FALL:
		if (pin_get_input(port->scl)) {
			scl_set_low(port->scl);
			pin_set_high(port->sda);
			port->state = READ_RISE;
		}
		break;
	case READ_ACK_RISE:
		pin_set_low(port->sda);
		scl_set_high(port->scl);
		port->state = READ_ACK_FALL;
		break;
	case STOP_DATA_FALL:
		pin_set_low(port->sda);
		scl_set_high(port->scl);
		port->state = I2C_STOP;
		break;
	case I2C_STOP:
		if (pin_get_input(port->scl)) {
			pin_set_high(port->sda);
			port->state = STOP_FALL;
		}
		break;
	case STOP_FALL:
		if (pin_get_input(port->sda) && pin_get_input(port->scl)) {
			scl_set_low(port->scl);
			port->state = STOP_RISE;
		}
		break;
	case STOP_RISE:
		scl_set_high(port->scl);
		port->state = IDLE;
		break;
	case WRITE_FALL:
		if (pin_get_input(port->scl)) {
			scl_set_low(port->scl);
			if (port->write_byte & 0x80) {
				pin_set_high(port->sda);
			} else {
				pin_set_low(port->sda);
			}
			port->write_byte <<= 1;
			if (port->bit_cnt-- == 0) { //0_o
				pin_set_high(port->sda);
				port->state = WRITE_ACK_RISE;
			} else {
				port->state = WRITE_RISE;
			}
		}
		break;
	case WRITE_RISE:
		scl_set_high(port->scl);
		port->state = WRITE_FALL;
		break;
	case WRITE_ACK_RISE:
		scl_set_high(port->scl);
		port->state = WRITE_ACK_FALL;
		if (port->operation == WRITE) {
			port->write_byte = *(port->data++);
			port->bit_cnt = 8;
		}
		break;
	case WRITE_ACK_FALL:
		if (pin_get_input(port->scl)) {
			uint32_t was_state = pin_get_input(port->sda);
			scl_set_low(port->scl);
			if (was_state) {
				port->state = STOP_DATA_FALL;
			} else {
				if (port->operation == WRITE) {
					if (port->write_byte & 0x80) {
						pin_set_high(port->sda);
					} else {
						pin_set_low(port->sda);
					}
					port->write_byte <<= 1;
					port->bit_cnt--;
					port->state = WRITE_RISE;
				} else {
					port->bit_cnt = 0;
					port->state = READ_RISE;
				}
			}
			if (port->data_cnt-- == 0) {
				port->state = STOP_DATA_FALL;
			}
		}
		break;
	case IDLE:
	default:
		break;
	}
}

void i2c_read(i2c_port_t *port, uint8_t addr, uint8_t *data, uint32_t count) {
	port->data = data;
	port->data_cnt = count;
	port->operation = READ;
	port->write_byte = (addr << 1) | 1;
	port->bit_cnt = 8;
	pin_set_high(port->sda);
	scl_set_high(port->scl);
	port->state = START;
}

void i2c_write(i2c_port_t *port, uint8_t addr, uint8_t *data, uint32_t count) {
	port->data = data;
	port->data_cnt = count;
	port->operation = WRITE;
	port->write_byte = (addr << 1) | 0;
	port->bit_cnt = 8;
	pin_set_high(port->sda);
	scl_set_high(port->scl);
	port->state = START;
}

static irq_return_t timer_handler(irq_nr_t irq_nr, void *data) {
	size_t i;
	REG_LOAD(((uint8_t *) AT91C_TC0_SR) + TIMER * sizeof(AT91S_TCB));
	for (i = 0; i < port_count; i++) {
		i2c_port_process(ports[i]);
	}
	tc_reset(TIMER);
	return IRQ_HANDLED;
}

void i2c_init(i2c_port_t *port) {
	ports[port_count++] = port;
	REG_STORE(AT91C_PIOA_PER, port->scl | port->sda);
	REG_STORE(AT91C_PIOA_PPUER, port->sda);
	REG_STORE(AT91C_PIOA_ODR, port->sda);

	REG_STORE(AT91C_PIOA_MDER, port->scl);
	REG_STORE(AT91C_PIOA_PPUER, port->scl);
	REG_STORE(AT91C_PIOA_SODR, port->scl);
	REG_STORE(AT91C_PIOA_OER, port->scl);

	port->state = IDLE;
}

static int i2c_unit_init(void) {
	tc_init(TIMER);
	/* MCLK/2, RC compare trigger */
	tc_config_input(TIMER, (uint32_t) AT91C_TC_CLKS_TIMER_DIV1_CLOCK);
	/* running on twice speed i2c_clock*/
	tc_set_limit(TIMER, (CONFIG_SYS_CLOCK/2)/(2 * I2C_CLOCK));
	tc_limit_int_enable(TIMER, timer_handler);
	tc_reset(TIMER);
	return 0;
}
