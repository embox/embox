/**
 * @file
 * @brief
 *
 * @date 10.01.2017
 * @author Alex Kalmuk
 */

#include <stdio.h>
#include <unistd.h>

#include <embox/unit.h>
#include <libs/ir.h>

/* TODO It is possible to make it as an option, but currently is not
 * very useful case */
#define ITERS 30

static void ir_test(struct ir_led *led, struct ir_receiver *rcv) {
	int i;
	int prev = 0, cur = 0;

	ir_led_enable(led);

	for (i = 0; i < ITERS; i++) {
		cur = ir_receiver_pulse_count(rcv);

		printf("Total number of pulses: %d\n\n"
			   "Difference (current - previous): %d\n\n",
				cur, cur - prev);
		prev = cur;

		sleep(1);
	}

	ir_led_disable(led);
}

int main(int argc, char *argv[]) {
	struct ir_led ir_led;
	struct ir_receiver ir_rcv;

	printf("IR sensors test start!\n"
			"This test prints every 1 second IR pulses count %d times\n",
			ITERS);

	ir_led_init(&ir_led, GPIO_PIN_3, GPIOD);
	ir_receiver_init(&ir_rcv, GPIO_PIN_1, GPIOD, EXTI1_IRQn);

	ir_test(&ir_led, &ir_rcv);

	return 0;
}
