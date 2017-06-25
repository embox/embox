/**
 * @file
 *
 * @date 14.07.2015
 * @author Alex Kalmuk
 */

#include <math.h>
#include <util/math.h>

#include <libfilters/dynamic_window.h>

static float data[WIN_SIZE] = {0};
static int index = 0;
static int middle = (WIN_SIZE + 1) / 2;

static float avg(int k) {
	float s = data[middle];
	int index_l, index_r;

	for (int i = 1; i <= k; i++) {
		index_r = (middle + i) % WIN_SIZE;
		index_l = (middle - i >= 0) ? (middle - i) : (middle - i + WIN_SIZE);

		s += data[index_l] + data[index_r];
	}

	return s / (2 * k + 1);
}

static void dk_calc(float K, int k, int *interval) {
	float s, r;
	
	s = avg(k);
	r = (2 * K) / sqrt(2 * k + 1);
	interval[0] = s - r;
	interval[1] = s + r;
}

static void intersect(int *a, int *b, int *res) {
	res[0] = max(a[0], b[0]);
	res[1] = min(a[1], b[1]);
}

float dyn_window_avg(float val, float K) {
	int interval[2], interval_new[2];
	float res = 0;
	int k;

	data[index] = val;

	k = 0;
	dk_calc(K, k, interval);

	while (interval[0] < interval[1]) {
		k++;
		dk_calc(K, k, interval_new);
		intersect(interval, interval_new, interval);
	}
	res = avg(k);

	index = (index + 1) % WIN_SIZE;
	middle = (middle + 1) % WIN_SIZE;

	return res;
}
