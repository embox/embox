/**
 *@file
 *@brief testing malloc
 *
 *@date 12.12.2010
 *@author Alexandr Kalmuk
 */

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define COUNT 100000

int main(void) {
	int i;
	struct timeval tv;
	struct tm* ptm;
	int* arr;
	int** ptr_arr;
	long start_time;
	long end_time;
	gettimeofday(&tv, NULL);
	ptm = localtime(&tv.tv_sec);
	start_time = tv.tv_usec / 1000;
	arr = (int*) malloc(sizeof(int) * COUNT);
	gettimeofday(&tv, NULL);
	end_time = tv.tv_usec / 1000;
	printf("%ld\n", end_time - start_time);
	ptr_arr = (int**) malloc(sizeof(int*) * COUNT);
	gettimeofday(&tv, NULL);
	ptm = localtime(&tv.tv_sec);
	start_time = tv.tv_usec / 1000;
	for (i = 0; i < COUNT; i++) {
		ptr_arr[i] = (int*) malloc(sizeof(int));
	}
	gettimeofday(&tv, NULL);
	end_time = tv.tv_usec / 1000;
	printf("%ld\n", end_time - start_time);
	for (i = 0; i < COUNT; i++) {
		free(ptr_arr[i]);
	}
	free(ptr_arr);
	free(arr);
	return 1;
}
