/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    11.12.2014
 */

#include <unistd.h>
#include <errno.h>

#include <kernel/sched.h>
#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_sched_wait.h>

#include <embox/cmd.h>
#include <framework/mod/options.h>

#define USE_LCD OPTION_GET(BOOLEAN, use_lcd)

#if USE_LCD
#include <drivers/gpio.h>
#include <drivers/lcd/lcd.h>
#else
#include <drivers/vt.h>
#include <drivers/diag.h>
#include <stdio.h>
#endif

EMBOX_CMD(exec);

#define ROAD_LENGTH 16
#define ROAD_WIDTH 2
#define LEVEL_PERIOD_MS 5
#define ROAD_UPDATE_MS 150
#define CAR_UPDATE_MS 130
#define OBSTACLE_SPACE 4
#define SCORE_PER_LEVEL 4

#define CAR_CHAR '>'
#define OBSTACLE_CHAR 'X'
#define SPACE_CHAR ' '

static char road[ROAD_WIDTH * ROAD_LENGTH];

static int car_pos, end, score, speed,count;

static void road_init(void) {
	for (int i = 0; i < ROAD_WIDTH; i++)
		for (int j = 0; j < ROAD_LENGTH; j++)
			road[i*ROAD_LENGTH + j] = SPACE_CHAR;

	road[car_pos*ROAD_LENGTH] = CAR_CHAR;
}

static void game_init(void) {
	car_pos = end = score = speed = count = 0;
	road_init();
}

#if USE_LCD
static void road_print(void) {
	for (int i = 0; i < ROAD_WIDTH*ROAD_LENGTH; i++) {
		lcd_putc(road[i]);
	}
}
#else
static void road_print(void) {
	int i, j;

	printf("%c%c2J",ESC,CSI);
	printf("%c%cH",ESC,CSI);

	for (i = 0; i < ROAD_WIDTH; i++) {
		putchar('|');
		for (j = 0; j < ROAD_LENGTH; j++)
			putchar(road[i * ROAD_LENGTH + j]);
		putchar('|');
		putchar('\n');
	}

	printf("\nscore: %i speed: %i\n", score, speed);
}
#endif

static int move_car(struct lthread* self) {
	int wait_res, is_button_clicked;

	goto lthread_resume(self, &&update);

update:
	if (end)
		return 0;

#if USE_LCD
	gpio_settings(GPIO_A, 0xff << 0, GPIO_MODE_INPUT);
	is_button_clicked = gpio_get_level(GPIO_A, 0xff << 0) & 0x01;
#else
	is_button_clicked = !diag_kbhit() && SPACE_CHAR == diag_getc();
#endif

	if (is_button_clicked) {
		road[car_pos*ROAD_LENGTH] = SPACE_CHAR;
		car_pos = (car_pos + 1) % ROAD_WIDTH;

		end = end || road[car_pos*ROAD_LENGTH] == OBSTACLE_CHAR;

		road[car_pos*ROAD_LENGTH] = CAR_CHAR;
		road_print();

		if (end)
			return 0;
	}

wait:
	wait_res = SCHED_WAIT_TIMEOUT_LTHREAD(0, CAR_UPDATE_MS);
	if (wait_res == -EAGAIN) {
		return lthread_yield(&&update, &&wait);
	}

	goto update;
}

static void increase_speed_and_score(void) {
	int is_obstacle = 0;

	for (int i = 0; i < ROAD_WIDTH; i++)
		is_obstacle = is_obstacle || road[i*ROAD_LENGTH] == OBSTACLE_CHAR;

	if (is_obstacle && ++score % SCORE_PER_LEVEL == 0)
		speed++;
}

static void move_road_array(void) {
	road[car_pos*ROAD_LENGTH] = SPACE_CHAR;

	for (int i = 0; i < ROAD_WIDTH; i++) {
		for (int j = 1; j < ROAD_LENGTH; j++)
			road[i*ROAD_LENGTH + j - 1] = road[i*ROAD_LENGTH + j];
		road[i*ROAD_LENGTH + ROAD_LENGTH - 1] = SPACE_CHAR;
	}

	road[car_pos*ROAD_LENGTH] = CAR_CHAR;
}

static void set_obtacle(void) {
	if (count++ == OBSTACLE_SPACE) {
		count = 0;
		road[(clock() % ROAD_WIDTH)*ROAD_LENGTH + ROAD_LENGTH - 1] = OBSTACLE_CHAR;
	}
}

static int move_road(struct lthread *self) {
	int wait_res;

	goto lthread_resume(self, &&update);

update:
	end = end || road[car_pos*ROAD_LENGTH + 1] == OBSTACLE_CHAR;
	if (end)
		return 0;

	increase_speed_and_score();
	move_road_array();
	set_obtacle();

	road_print();

wait:
	wait_res = SCHED_WAIT_TIMEOUT_LTHREAD(0, ROAD_UPDATE_MS - speed * LEVEL_PERIOD_MS);
	if (wait_res == -EAGAIN) {
		return lthread_yield(&&update, &&wait);
	}

	goto update;
}

static int exec(int argc, char **argv) {
	struct lthread lt_car, lt_road;

	game_init();
	road_print();

	lthread_init(&lt_road, move_road);
	lthread_init(&lt_car, move_car);

	lthread_launch(&lt_road);
	lthread_launch(&lt_car);

	while (!end) {
		schedule();
	}

	lthread_reset(&lt_car);
	lthread_reset(&lt_road);

	return 0;
}
