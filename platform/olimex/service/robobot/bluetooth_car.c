/**
 * @file
 * @brief
 *
 * @date 15.07.11
 * @author Anton Kozlov
 */

#include <stdint.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <drivers/bluetooth/bluetooth.h>
#include <drivers/at91sam7s256.h>
#include <drivers/pins.h>
#include <util/callback.h>
#include <drivers/at91_olimex_debug_led.h>

#include <service/robobot_car.h>

#define RIGHT_PIN AT91C_PIO_PA12
#define LEFT_PIN AT91C_PIO_PA13

#define FORWARD_PIN AT91C_PIO_PA14
#define BACKWARD_PIN AT91C_PIO_PA0

EMBOX_UNIT_INIT(robobot_bluetooth_car);

static int car_bt_disconnect(void);
static int car_bt_connect(void);
static int car_bt_read(int len, void *data);

static uint8_t determ_ack[ROBOBOT_DETERM_ACK_SIZE] =
	{0x03, 0x00, 0x02, 0x0d, 0x02};

//#define MOTOR_DRY_RUN

static void turn_right(void) {
#ifndef MOTOR_DRY_RUN
	pin_set_output(RIGHT_PIN);
	pin_clear_output(LEFT_PIN);
#endif
	pin_clear_output(OLIMEX_SAM7_LED1);
}

static void turn_left(void) {
#ifndef MOTOR_DRY_RUN
	pin_clear_output(RIGHT_PIN);
	pin_set_output(LEFT_PIN);
#endif
	pin_clear_output(OLIMEX_SAM7_LED1);
}

static void turn_none(void) {
#ifndef MOTOR_DRY_RUN
	pin_clear_output(RIGHT_PIN);
	pin_clear_output(LEFT_PIN);
#endif
	pin_set_output(OLIMEX_SAM7_LED1);
}

static void go_forw(void) {
#ifndef MOTOR_DRY_RUN
	pin_set_output(FORWARD_PIN);
	pin_clear_output(BACKWARD_PIN);
#endif
	pin_clear_output(OLIMEX_SAM7_LED2);
}

static void go_back(void) {
#ifndef MOTOR_DRY_RUN
	pin_clear_output(FORWARD_PIN);
	pin_set_output(BACKWARD_PIN);
#endif
	pin_clear_output(OLIMEX_SAM7_LED2);
}

static void go_none(void) {
#ifndef MOTOR_DRY_RUN
	pin_clear_output(FORWARD_PIN);
	pin_clear_output(BACKWARD_PIN);
#endif
	pin_set_output(OLIMEX_SAM7_LED2);
}

static int robobot_handle(uint8_t *buff) {
	return (buff[0] == 0x42 && buff[1] == 0x24);
}

#define FORW_THRESHOLD 45
#define TURN_THRESHOLD 65
static int8_t speed;
static int8_t direction;

static void robobot_handle_car(uint8_t *buff) {
	speed = (int8_t) buff[0];
	direction = (int8_t) buff[1];

	if (speed > FORW_THRESHOLD) {
	    go_forw();
	} else if (speed < -FORW_THRESHOLD) {
	    go_back();
	} else {
	    go_none();
	}

	if (direction > TURN_THRESHOLD) {
	    turn_right();
	} else if (direction < -TURN_THRESHOLD) {
	    turn_left();
	} else {
	    turn_none();
	}
}

static int robobot_stamp(uint8_t *buff) {
	uint8_t id[4] = {0x02, 0, 0, 0x0d};
	int i;
	for (i = 0; i < 4; i++) {
	    if (id[i] != buff[i]) {
		return 0;
	    }
	}
	return 1;
}

static int car_bt_read(int len, void *_buff) {
	uint8_t *buff = _buff;
	if (robobot_handle(buff)) {
		robobot_handle_car(buff + ROBOBOT_HEADER_SIZE);
	} else if (robobot_stamp(buff)) {
		/* send robobot_car id */
		bluetooth_write(determ_ack, ROBOBOT_DETERM_ACK_SIZE);
	}
	bluetooth_read(ROBOBOT_MSG_SIZE);
	return 0;
}

static int car_bt_connect(void) {
	bluetooth_write((uint8_t *) "test", 4);
	bluetooth_read(ROBOBOT_MSG_SIZE);
	pin_set_output(OLIMEX_SAM7_LED1 | OLIMEX_SAM7_LED2);
	CALLBACK_REG(bt_state, car_bt_disconnect);
	return 0;
}

static int car_bt_disconnect(void) {
	pin_clear_output(OLIMEX_SAM7_LED1 | OLIMEX_SAM7_LED2);
	CALLBACK_REG(bt_state, car_bt_connect);
	return 0;
}

static int robobot_bluetooth_car(void) {
	CALLBACK_REG(bt_rx, car_bt_read);
	CALLBACK_REG(bt_state, car_bt_connect);

	REG_STORE(AT91C_PIOA_PDR, RIGHT_PIN | LEFT_PIN |
			FORWARD_PIN | BACKWARD_PIN);
	REG_STORE(AT91C_PIOA_ASR, BACKWARD_PIN);
	REG_STORE(AT91C_PIOA_BSR, RIGHT_PIN | LEFT_PIN | FORWARD_PIN );

	pin_config_output(RIGHT_PIN | LEFT_PIN | FORWARD_PIN | BACKWARD_PIN);
	pin_config_output(OLIMEX_SAM7_LED1 | OLIMEX_SAM7_LED2);

	return 0;
}
