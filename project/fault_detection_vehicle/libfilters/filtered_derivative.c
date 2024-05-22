/**
 * @file
 *
 * @date 07 july 2015
 * @author: Anton Bondarev
 */

#include <libfilters/filtered_derivative.h>

float filtered_derivative(float inc) {
	static float f_dv[FD_WIN_SIZE] = {0};
	static int index_l = 0;
	static int index_h = FD_WIN_SIZE / 2;
	static int index = 0;
	static float diff = 0;

	diff += ((inc - 2 * f_dv[index_h] + f_dv[index_l]) / FD_WIN_SIZE);
	f_dv[index] = inc;
	index = (index + 1) % FD_WIN_SIZE;
	index_h = (index_h + 1) % FD_WIN_SIZE;
	index_l = (index_l + 1) % FD_WIN_SIZE;

	return diff;
}

int filtered_derivative_check(float val) {
	return (val < -FD_THRESHOLD || val > FD_THRESHOLD);
}
