/**
 * @file
 * @brief The simple game demonstrating how lthreads work.
 *
 * See main(), move_road() and move_car() functions for the lthread usage
 * example.
 *
 * Related documentation:
 *  - lthread.h
 *  - lthread/waitq.h
 *  - lthread_sched_wait.h
 *
 * @author  Vita Loginova
 * @date    11.12.2014
 */

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <kernel/lthread/lthread.h>
#include <kernel/lthread/lthread_sched_wait.h>

#include "race.h"

#define is_obstacle(idx) road[idx] == RACE_OBSTACLE

static char road[RACE_ROAD_WDT * RACE_ROAD_LEN];
static int car_line_nr, is_game_over, speed, step;

static void race_init(void) {
	car_line_nr = is_game_over = speed = step = 0;

	for (int i = 0; i < RACE_ROAD_WDT; i++)
		for (int j = 0; j < RACE_ROAD_LEN; j++)
			road[i*RACE_ROAD_LEN + j] = RACE_SPACE;

	road[car_line_nr*RACE_ROAD_LEN] = RACE_CAR;
}

static void race_update_road(void) {
	for (int i = 0; i < RACE_ROAD_WDT; i++) {
		for (int j = 1; j < RACE_ROAD_LEN; j++)
			road[i*RACE_ROAD_LEN + j - 1] = road[i*RACE_ROAD_LEN + j];
		road[i*RACE_ROAD_LEN + RACE_ROAD_LEN - 1] = RACE_SPACE;
	}

	step++;
	if (!(step % RACE_OBSTACLE_STEP)) {
		road[(rand() % RACE_ROAD_WDT + 1)*RACE_ROAD_LEN - 1] = RACE_OBSTACLE;
	}

	road[car_line_nr*RACE_ROAD_LEN] = RACE_CAR;
}

static int move_road(struct lthread *self) {
	int to_wait;

	/* Multiple entry points are provided by lthread_resume() and
	 * lthread_yield() functions. */
	goto *lthread_resume(self, &&update);

update:
	is_game_over |= is_obstacle(car_line_nr*RACE_ROAD_LEN + 1);
	if (is_game_over)
		return 0;

	race_update_road();
	race_print_road(road);

wait:
	to_wait = RACE_ROAD_UPD_MS - (step / RACE_LVL_STEP) * RACE_LVLUP_MS;

	if (SCHED_WAIT_TIMEOUT_LTHREAD(self, 0, to_wait) == -EAGAIN) {
		/* Next time the function will start from the 'wait' label. Return
		 * in order to not to block other threads. */
		return lthread_yield(&&update, &&wait);
	}

	goto update;
}

static int move_car(struct lthread* self) {
	goto *lthread_resume(self, &&update);

update:
	if (is_game_over)
		return 0;

	if (race_is_car_moved()) {
		road[car_line_nr*RACE_ROAD_LEN] = RACE_SPACE;
		car_line_nr = (car_line_nr + 1) % RACE_ROAD_WDT;
		road[car_line_nr*RACE_ROAD_LEN] = RACE_CAR;

		race_print_road(road);

		is_game_over |= is_obstacle(car_line_nr*RACE_ROAD_LEN);
		if (is_game_over)
			return 0;
	}

wait:
	if (SCHED_WAIT_TIMEOUT_LTHREAD(self, 0, RACE_CAR_UPD_MS) == -EAGAIN) {
		return lthread_yield(&&update, &&wait);
	}

	goto update;
}

int main(int argc, char **argv) {
	struct lthread lt_car, lt_road;
	int score;

	race_init();
	race_print_road(road);

	lthread_init(&lt_road, move_road);
	lthread_init(&lt_car, move_car);

	lthread_launch(&lt_road);
	lthread_launch(&lt_car);

	lthread_join(&lt_road);
	lthread_join(&lt_car);

	score = (step - RACE_ROAD_LEN) / RACE_OBSTACLE_STEP;
	race_print_score(score);

	return 0;
}
