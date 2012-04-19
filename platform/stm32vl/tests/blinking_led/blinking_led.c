/**
 * @file
 * @brief
 *
 * @date 18.03.12
 * @author Anton Kozlov
 */

#include <embox/test.h>
#include <unistd.h>
#include <hal/reg.h>

EMBOX_TEST(blinking_led);

#define RCC_APB1RSTR  0x40021010
#define RCC_APB1PWR   0x10000000
#define RCC_APB2ENR   0x40021018
#define RCC_APB2GPIOC 0x00000010

#define GPIOC 0x40011000 /* port C */
#define GPIOC_CRH (GPIOC + 0x04) /* port configuration register high */
#define LED_PORT_ODR (GPIOC + 0x0c) /* port output data register */

#define LED_BLUE (1 << 8) /* port C, pin 8 */
#define LED_GREEN (1 << 9) /* port C, pin 9 */

#define DELAY   0x25000

static void delay(int d) {
	while (-- d) {
	}
}

static void led1_on(void) {
	 REG_STORE(LED_PORT_ODR, LED_BLUE | LED_GREEN);
}

static void led1_off(void) {
	 REG_STORE(LED_PORT_ODR, 0);
}

static int blinking_led(void) {
	static int count = 3;
	REG_STORE(RCC_APB1RSTR,RCC_APB1PWR);
	REG_STORE(RCC_APB2ENR,RCC_APB2GPIOC);
	REG_STORE(GPIOC_CRH,0x44444411);

	while (count--) {
		led1_on();
		delay(DELAY);
		led1_off();
		delay(DELAY);
	}

	return 0;
}
