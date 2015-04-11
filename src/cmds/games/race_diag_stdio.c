/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    08.04.2015
 */

#include <drivers/vt.h>
#include <drivers/diag.h>
#include <stdio.h>

#include "race.h"

void race_print_road(const char *road) {
	int i, j;

	printf("%c%c2J", ESC, CSI);
	printf("%c%cH", ESC, CSI);

	for (i = 0; i < RACE_ROAD_WDT; i++) {
		putchar('|');
		for (j = 0; j < RACE_ROAD_LEN; j++)
			putchar(road[i * RACE_ROAD_LEN + j]);
		putchar('|');
		putchar('\n');
	}
}

void race_print_score(const int score) {
	printf("%c%c2J", ESC, CSI);
	printf("%c%cH", ESC, CSI);
	printf("GAME OVER\nscore: %i\n", score);
}

int race_is_car_moved(void) {
	return !diag_kbhit() && ' ' == diag_getc();
}
