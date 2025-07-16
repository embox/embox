/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    08.04.2015
 */

#include <stdlib.h>
#include <string.h>
#include <drivers/gpio.h>
#include <drivers/lcd/lcd.h>

#include "race.h"

void race_print_road(const char *road) {
	for (int i = 0; i < RACE_ROAD_WDT*RACE_ROAD_LEN; i++) {
		lcd_putc(road[i]);
	}
}

/* TODO: Fix this hard code stuff. */
void race_print_score(const int score) {
	char game_over_str[] = "    GAME OVER   \n    SCORE: "; /* 5 chars left */
	char score_str[5];
	int score_strlen;

	for (int i = 0; i < sizeof(game_over_str) - 1; i++) {
		lcd_putc(game_over_str[i]);
	}

	itoa(score % 10000, score_str, 10);
	score_strlen = strlen(score_str);

	for (int i = 0; i < score_strlen; i++) {
		lcd_putc(score_str[i]);
	}

	for (int i = 0; i < 5 - score_strlen; i++) {
		lcd_putc(' ');
	}
}

int race_is_car_moved(void) {
	gpio_setup_mode(GPIO_PORT_A, 0xff << 0, GPIO_MODE_IN);
	return gpio_get(GPIO_PORT_A, 0xff << 0) & 0x01;
}
