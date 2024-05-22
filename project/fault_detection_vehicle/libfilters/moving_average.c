/**
 * @file
 *
 * @date 07 yuly 2015
 * @author: Anton Bondarev
 */


#define INC_WINDOW_SIZE 50
#define EMA_ALPHA ((float) 2.0 / (float) (INC_WINDOW_SIZE + 1))

static float incs[INC_WINDOW_SIZE] = {0};
static int inc_index = 0;
static float average_inc = 0;

float moving_average(float inc) {
	average_inc += (inc - incs[inc_index]) / INC_WINDOW_SIZE;
	incs[inc_index] = inc;
	inc_index = (inc_index + 1) % INC_WINDOW_SIZE;
	return average_inc;
}

float exp_moving_average(float inc) {
	static float ema = 0;
	ema += (inc - ema) * EMA_ALPHA;
	return ema;
}
