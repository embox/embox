#include <stdio.h>
#include <time.h>

#define ITERS     1000

int main(int argc, char **argv) {
	int i;
	clock_t start;

	printf("Starting simple \"for\" cycle %d iters\n", ITERS);
	start = clock();
	for (i = 0; i < ITERS; i++) {
	}
	printf("Total clocks for %d iters: %ld\n", ITERS, clock() - start);

	printf("\nStarting %d WFI's cycle\n", ITERS);
	start = clock();
	for (i = 0; i < ITERS; i++) {
		__asm__ __volatile__ ("wfi");
	}
	printf("Total clocks for %d WFI's: %ld\n", ITERS, clock() - start);

	return 0;
}
