/**
 *@file
 *@brief testing malloc
 *
 *@date 12.12.2010
 *@author Alexandr Kalmuk
 */

#include <stdio.h>
#include <windows.h>
#include <time.h>

#define SIZE 1000

int main(void) {
	int i;
	int* arr;
	int** ptr_arr;
	float delta_time;
	float pseudu_delta_time;
	float cycle_time;
	long long gstart_time;
	long long glast_time;
	long long freq;
	/* Get current count*/
	QueryPerformanceCounter((LARGE_INTEGER *) &gstart_time);
	/*Get processor freq (count of ticks(in second))*/
	QueryPerformanceFrequency((LARGE_INTEGER *) &freq);
	arr = (int*) malloc(sizeof(int) * SIZE);
	QueryPerformanceCounter((LARGE_INTEGER *) &glast_time);
	/*Time in milliseconds*/
	delta_time = (double) ((double) ((glast_time - gstart_time) * 1000)
			/ (double) freq);
	printf("%f\n", delta_time);

	ptr_arr = (int**) malloc(sizeof(int*) * SIZE);
	QueryPerformanceCounter((LARGE_INTEGER *) &gstart_time);
	for (i = 0; i < SIZE; i++) {
		ptr_arr[i] = (int*) malloc(sizeof(int));
	}
	QueryPerformanceCounter((LARGE_INTEGER *) &glast_time);
	/*Time taking into account time that cycle "for{}" worked*/
	pseudu_delta_time = (double) ((double) ((glast_time - gstart_time) * 1000)
			/ (double) freq);
	QueryPerformanceCounter((LARGE_INTEGER *) &gstart_time);
	for (i = 0; i < SIZE; i++) {
	}
	QueryPerformanceCounter((LARGE_INTEGER *) &gstart_time);
	cycle_time = (double) ((double) ((glast_time - gstart_time) * 1000)
			/ (double) freq);
	delta_time = pseudu_delta_time - cycle_time;
	printf("%f\n", delta_time);
	for (i = 0; i < SIZE; i++) {
		free(ptr_arr[i]);
	}
	free(ptr_arr);
	free(arr);
	return 1;
}
