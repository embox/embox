#include <unistd.h>
#include <stdio.h>
#include <embox/cmd.h>
#include <drivers/vt.h>
#include <drivers/diag.h>
#include <kernel/lthread/lthread.h>

#include <kernel/sched.h>
#include <kernel/lthread/lthread_sched_wait.h>

#include <errno.h>

EMBOX_CMD(exec);

#define ROAD_LENGTH 20
#define ROAD_WIDTH 2
#define LEVEL_PERIOD_MS 5
#define ROAD_UPDATE_MS 100
#define CAR_UPDATE_MS 10
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

static void *move_car(void* arg) {
	int wait_res;
	void *start_label = lthread_start_label_get();

	if (start_label)
		goto *start_label;

update:
	if (end)
		return 0;

	if (!diag_kbhit() && SPACE_CHAR == diag_getc()) {
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
		lthread_start_label_set(&&wait);
		return 0;
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

static void *move_road(void* arg) {
	int wait_res;
	void *start_label = lthread_start_label_get();

	if (start_label)
		goto *start_label;

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
		lthread_start_label_set(&&wait);
		return 0;
	}

	goto update;
}

static int exec(int argc, char **argv) {
	struct lthread lt_car, lt_road;

	game_init();
	road_print();

	lthread_init(&lt_road, move_road, NULL);
	lthread_init(&lt_car, move_car, NULL);

	lthread_launch(&lt_road);
	lthread_launch(&lt_car);

	while (!end) {
		sleep(0);
	}

	lthread_delete(&lt_car);
	lthread_delete(&lt_road);

	return 0;
}
