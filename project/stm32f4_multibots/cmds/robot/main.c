/**
 * @file
 * @brief
 *
 * @date 11.01.2017
 * @author Alex Kalmuk
 */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include <framework/mod/options.h>

#include <libs/nrf24.h>
#include <drivers/sensors/gy_30.h>
#include <libs/stepper_motor.h>
#include <libs/ir.h>

/* When number of IR pulses received during 1 sec exceeds this
 * threshold, it means the obstacle is somewhere near in front of us */
#define IR_OBSTACLE_THRESHOLD 10

#define NRF24_PLAYLOAD_LEN    16

#define GY_30_I2C_BUS_NR OPTION_GET(NUMBER, gy_30_i2c_bus_nr)

struct light_robot {
	struct stepper_motor motor1;
	struct stepper_motor motor2;
	struct ir_led ir_led;
	struct ir_receiver ir_rcv;
	bool stopped;
	int move;
	int direction;
};

static void init_leds(void) {
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
}

static void *light_sensor_loop(void *arg) {
	uint16_t level;

	while (1) {
		level = gy_30_read_light_level(GY_30_I2C_BUS_NR);
		printf("GY-30: light (lx) - %d\n", level);
		sleep(1);
	}

	return NULL;
}

/* Detectes obstacles and stop the robot if any obstacle found */
static void *ir_sensor_loop(void *arg) {
	struct light_robot *robot = arg;
	struct ir_receiver *rcv = &robot->ir_rcv;
	int prev = 0, cur = 0;

	while (1) {
		cur = ir_receiver_pulse_count(rcv);

		printf("IR: pulses count (current - previous) - %d\n",
				cur - prev);
		if (!robot->stopped) {
			if (cur - prev >= IR_OBSTACLE_THRESHOLD) {
				printf("Robot STOPPED\n");
				robot->stopped = true;
			}
		}
		prev = cur;
		sleep(1);
	}

	return NULL;
}

static int nrf24_handle_msg(struct light_robot *robot,
							uint8_t *msg, size_t len) {
	printf("\nReceive command: %s\n", msg);

	if (0 == strncmp((char *) msg, "move", 4)) {
		char *direction = (char *) &msg[5];

		if (0 == strncmp(direction, "up", 2)) {
			robot->stopped = false;
			robot->move = 1;
			robot->direction = MOTOR_RUN_FORWARD;
		} else if (0 == strncmp(direction, "down", 4)) {
			robot->stopped = false;
			robot->move = 1;
			robot->direction = MOTOR_RUN_BACKWARD;
		}
	} else if (0 == strncmp((char *) msg, "stop", 4)) {
		robot->stopped = true;
	} else if (0 == strncmp((char *) msg, "start", 5)) {
		robot->stopped = false;
	} else if (0 == strncmp((char *) msg, "light", 5)) {
		uint16_t level;
		int tmp = 0;

		level = gy_30_read_light_level(GY_30_I2C_BUS_NR);

		usleep(100000);

		while (1) {
			nrf24_send((uint8_t *) &level);
			while (nrf24_isSending())
				;
			tmp = nrf24_lastMessageStatus();
			if (tmp == NRF24_TRANSMISSON_OK) {
				printf("Transmission OK\n");
				break;
			} else {
				printf("Transmission ERROR\n");
				usleep(500000);
			}
		}
	}

	return 0;
}

static void *nrf24_loop(void *arg) {
	struct light_robot *robot = arg;
	uint8_t rx_buf[NRF24_PLAYLOAD_LEN];

	while (1) {
		nrf24_powerUpRx();

		printf("NRF24 START WAITING\n");

		while (!nrf24_dataReady())
			;
		nrf24_getData(rx_buf);

		nrf24_handle_msg(robot, rx_buf, NRF24_PLAYLOAD_LEN);

		sleep(1);
	}

	return NULL;
}

static int robot_config_nrf24(uint8_t *tx_addr, uint8_t *rx_addr,
								uint8_t chan, int payload_len) {
	nrf24_init();

	if (nrf24_config(chan, payload_len) < 0) {
		return -1;
	}

	nrf24_rx_address(rx_addr);
	nrf24_tx_address(tx_addr);

	nrf24_powerUpRx();

	return 0;
}

static void robot_config_motors(struct stepper_motor *motor1,
								struct stepper_motor *motor2) {
	motor_init(motor1, GPIO_PIN_0, GPIO_PIN_2, GPIO_PIN_4, GPIO_PIN_6, GPIOD);
	motor_init(motor2, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12, GPIO_PIN_14, GPIOE);
	motor_set_speed(motor1, MOTOR_MAX_SPEED);
	motor_set_speed(motor2, MOTOR_MAX_SPEED);
}

static int robot_config_gy_30(int mode) {
	gy_30_setup_mode(GY_30_I2C_BUS_NR, mode);

	return 0;
}

static void main_loop(void) {
	pthread_t light_th, ir_th, nrf24_th;
	int res;
	struct light_robot robot;
	uint8_t tx_addr[5] = {0xAA,0xBB,0xCC,0xDD,0x01};
	uint8_t rx_addr[5] = {0xEE,0xFF,0xAA,0xBB,0x02};

	/* TODO Each robot must set it's own rx and tx address */
	if (robot_config_nrf24(tx_addr, rx_addr, 96, NRF24_PLAYLOAD_LEN) < 0) {
		printf("NRF24 configuration error. Probably, the module is disconnected\n");
		return;
	}
	printf("NRF24L01+ radio module configured\n");

	robot_config_motors(&robot.motor1, &robot.motor2);
	printf("Stepper motors configured\n");

	if (robot_config_gy_30(BH1750_CONTINUOUS_HIGH_RES_MODE) < 0) {
		printf("GY-30 initialization failed\n");
		return;
	}
	printf("GY-30 configured\n");

	ir_led_init(&robot.ir_led, GPIO_PIN_3, GPIOD);
	ir_receiver_init(&robot.ir_rcv, GPIO_PIN_1, GPIOD, EXTI1_IRQn);
	printf("IR LED (TSAL) and Receiver (TSOP) are configured\n");

	res = pthread_create(&light_th, 0, light_sensor_loop, NULL);
	if (res != 0) {
		printf("Light thread creation failure\n");
		return;
	}
	pthread_detach(light_th);

	res = pthread_create(&ir_th, 0, ir_sensor_loop, &robot);
	if (res != 0) {
		printf("IR thread creation failure\n");
		return;
	}
	pthread_detach(ir_th);

	res = pthread_create(&nrf24_th, 0, nrf24_loop, &robot);
	if (res != 0) {
		printf("NRF24 thread creation failure\n");
		return;
	}
	pthread_detach(nrf24_th);

	robot.move = 0;
	while (1) {
		if (!robot.stopped) {
			if (robot.move != 0) {
				motor_do_steps2(&robot.motor1, &robot.motor2,
								MOTOR_STEPS_PER_REVOLUTION / 4,
								robot.direction);
			}
			sleep(0);
		}
	}
}

int main(int argc, char *argv[]) {
	printf("Robot test start!\n");

	init_leds();
	BSP_LED_Toggle(LED6);

	main_loop();

	return 0;
}
