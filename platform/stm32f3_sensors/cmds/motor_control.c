/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    31.05.2015
 */

#include <stdio.h>
#include <unistd.h>
#include <stm32f3xx.h>
#include <stm32f3_discovery.h>
#include <stm32f3xx_hal_gpio.h>
#include <stm32f3xx_hal_rcc.h>

#include <stm32f3_discovery_gyroscope.h>
#include <stm32f3_discovery_accelerometer.h>

#define ACC_ARRAY_SIZE (512 * 4 + 128)

static int16_t x_values[ACC_ARRAY_SIZE];
static int16_t y_values[ACC_ARRAY_SIZE];


/* motor 1*/
#define MOTOR_ENABLE1  GPIO_PIN_3
#define MOTOR_INPUT1   GPIO_PIN_5
#define MOTOR_INPUT2   GPIO_PIN_7
/* motor 2 */
#define MOTOR_ENABLE2  GPIO_PIN_2
#define MOTOR_INPUT3   GPIO_PIN_4
#define MOTOR_INPUT4   GPIO_PIN_6

struct motor {
	GPIO_TypeDef  *GPIOx;
	uint16_t enable;
	uint16_t input[2];
	/* 10, 01, 11 */
	uint8_t enabled_inputs:2;
};

enum motor_run_direction {
	MOTOR_RUN_LEFT,
	MOTOR_RUN_RIGHT
};

static struct motor motor1;
static struct motor motor2;

static void stm32f3_delay(uint32_t delay) {
	while(delay--)
		;
}

static void init_pins(GPIO_TypeDef  *GPIOx, uint16_t pins) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Pin = pins;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void motor_init(struct motor *m, GPIO_TypeDef  *GPIOx, 
		uint16_t enable, uint16_t in1, uint16_t in2, uint8_t mask) {
	m->GPIOx = GPIOx;
	m->enable = enable;
	m->input[0] = in1;
	m->input[1] = in2;
	m->enabled_inputs = mask;
}

static void motor_enable(struct motor *m) {
	HAL_GPIO_WritePin(GPIOD, m->enable, GPIO_PIN_SET);
}

#if 0
static void motor_disable(struct motor *m) {
	HAL_GPIO_WritePin(GPIOD, m->enable, GPIO_PIN_RESET);
}
#endif

static void motor_run(struct motor *m, enum motor_run_direction dir) {
	uint8_t input;

	if (dir == MOTOR_RUN_LEFT) {
		input = 0;
	} else if (dir == MOTOR_RUN_RIGHT) {
		input = 1;
	}
	HAL_GPIO_WritePin(GPIOD, m->input[!input], GPIO_PIN_RESET);
	stm32f3_delay(0xFF); /* FIXME may be it is not needed */
	HAL_GPIO_WritePin(GPIOD, m->input[input], GPIO_PIN_SET);
}

static void motor_stop(struct motor *m) {
	stm32f3_delay(1000);
	HAL_GPIO_WritePin(GPIOD, m->enable, GPIO_PIN_RESET);
	stm32f3_delay(1000);
	HAL_GPIO_WritePin(GPIOD, m->input[0], GPIO_PIN_RESET);
	stm32f3_delay(1000);
	HAL_GPIO_WritePin(GPIOD, m->input[1], GPIO_PIN_RESET);
	stm32f3_delay(1000);
}

static void acc_test(void) {
    int16_t buf[3] = {0};

    for (size_t l = 0; l < 100; l++) {
        for (size_t m = 0; m < ACC_ARRAY_SIZE; m++) {
            BSP_ACCELERO_GetXYZ(buf);

            x_values[m] = buf[0];
            y_values[m] = buf[1];

            //stm32f3_delay(50);
        }
		motor_stop(&motor1);
        printf(">>");
    }
}

int main(void) {
	int res;

	HAL_Init();

	__GPIOD_CLK_ENABLE();

	res = BSP_ACCELERO_Init();
    if (res != HAL_OK) {
        printf("BSP_ACCLEERO_Init failed, returned %d\n", res);
        return -1;
    }

	init_pins(GPIOD, MOTOR_ENABLE1 | MOTOR_INPUT1 | MOTOR_INPUT2 |
			MOTOR_ENABLE2 | MOTOR_INPUT3 | MOTOR_INPUT4);

	motor_init(&motor1, GPIOD, MOTOR_ENABLE1, MOTOR_INPUT1, 
		MOTOR_INPUT2, 0x3);
	motor_init(&motor2, GPIOD, MOTOR_ENABLE2, MOTOR_INPUT3,
		MOTOR_INPUT4, 0x3);

	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
	
	motor_enable(&motor1);
	while(BSP_PB_GetState(BUTTON_USER) != SET)
		;
	motor_run(&motor1, MOTOR_RUN_RIGHT);

	acc_test();

	//motor_enable(&motor2);
	//while(1) {
	//	while(BSP_PB_GetState(BUTTON_USER) != SET)
	//		;
	//	motor_run(&motor1, MOTOR_RUN_LEFT);
	//	//motor_run(&motor2, MOTOR_RUN_LEFT);
	//	stm32f3_delay(0xFFFF);
	//	while(BSP_PB_GetState(BUTTON_USER) != SET)
	//		;
	//	motor_run(&motor1, MOTOR_RUN_RIGHT);
	//	//motor_run(&motor2, MOTOR_RUN_RIGHT);
	//	stm32f3_delay(0xFFFF);
	//}

	return 0;
}
