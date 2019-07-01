/**
 * @file memtest.c
 * @brief Simple utility to measure memory performance
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 21.03.2019
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include <framework/mod/options.h>
#include <mem/vmem.h>

static void print_help(char **argv) {
	assert(argv);
	assert(argv[0]);
	printf("Usage: %s [-nh] [-l LENGHT]\n", argv[0]);
	printf("\tOptions: -n Test non-cached memory\n");
	printf("\t         -l LENGTH Number of bytes to be operated "
			"(default 0x1000000 for memcpy() and 0x2000 for heap)\n");
	printf("\t         -r REPEAT Number of repeats (default 64 for memcpy() and 2048 for heap)\n");
	printf("\t         -p Run test for heap (a.k.a. pyramid)\n");
	printf("\t         -q Maximum number of heap regions\n");
	printf("\t         -s Random seed\n");
}

/* Heap-related stuff */
static int max_region_len;
static int random_seed;
static int heap_regions;

#define SCREEN_WIDTH 80

#define MAX_HEAP_REGIONS OPTION_GET(NUMBER,max_heap_regions)
static struct {
	size_t size;
	void *ptr;
	uint32_t hash;
} regions[MAX_HEAP_REGIONS];

static int region_hash(int k) {
	uint32_t res = 0;

	if (regions[k].ptr == 0) {
		return 0;
	}

	for (int i = 0; i < regions[k].size; i++) {
		res ^= ((uint8_t *) regions[k].ptr)[i] << (8 * (i % 4));
	}

	return res;
}

static int fill_region(int k) {
	assert(regions[k].ptr == NULL);

	regions[k].size = random() % max_region_len;

	regions[k].ptr = malloc(1 + regions[k].size);

	if (regions[k].ptr == NULL && regions[k].size > 0) {
		printf("\nFailed to allocate memory\n");
		return 0;
	}

	for (int i = 0; i < regions[k].size; i++) {
		((uint8_t *) regions[k].ptr)[i] = random() % 256;
	}

	regions[k].hash = region_hash(k);

	return 0;
}

static int free_region(int k) {
	if (regions[k].hash != region_hash(k)) {
		printf("%d\n", k);
		printf("\nError! Data modified somewhere @ %p .. %p\n",
				regions[k].ptr, regions[k].ptr + regions[k].size);
	}

	free(regions[k].ptr);

	regions[k].ptr = 0;

	return 0;
}

static int heap_test(int repeat_num, int len) {
	int k = 0;
	max_region_len = len;

	if (random_seed == 0) {
		random_seed = random();
	}

	srandom(random_seed);

	if (heap_regions > MAX_HEAP_REGIONS || heap_regions < 0) {
		heap_regions = MAX_HEAP_REGIONS;
	}

	printf("Call malloc()/free() %d times; using memory regions up to %d bytes\n",
			repeat_num, len);
	printf("Up to %d regions active at the same time\n", heap_regions);
	printf("Random seed =0x%08x\n", random_seed);

	memset(regions, 0, sizeof(regions));

	for (int i = 0; i < repeat_num; i++) {
		if (i % (repeat_num / SCREEN_WIDTH) == 0) {
			printf(".");
			fflush(stdout);
		}

		k = random() % heap_regions;

		if (regions[k].ptr != 0) {
			free_region(k);
		} else {
			fill_region(k);
		}
	}

	/* Clean up */
	for (int i = 0; i < heap_regions; i++) {
		if (regions[i].ptr != 0) {
			free_region(i);
		}
	}

	printf("\nTest finished!\n");

	return 0;
}

int main(int argc, char **argv) {
	int opt, nocache = 0, err;
	size_t len = 0;
	int seconds;
	int repeat = 0;
	bool do_heap_test = false;
	int flags;

	struct timespec time_post;
	struct timespec time_pre;

	void *r_addr1, *r_addr2;
	size_t r_len1, r_len2;

	random_seed = 0;
	heap_regions = 128;

	while (-1 != (opt = getopt(argc, argv, "pnhl:r:s:q:"))) {
		switch (opt) {
			case 'n':
				nocache = 1;
				break;
			case 'l':
				len = strtol(optarg, NULL, 0);
				break;
			case 'r':
				repeat = strtol(optarg, NULL, 0);
				break;
			case 'p':
				do_heap_test = true;
				break;
			case 'q':
				heap_regions = strtol(optarg, NULL, 0);
				break;
			case 's':
				random_seed = strtol(optarg, NULL, 0);
				break;
			case 'h':
				print_help(argv);
				return 0;
		}
	}

	if (do_heap_test) {
		/* Default values for heap test */
		if (len == 0) {
			len = 8192;
		}

		if (repeat == 0) {
			repeat = 2048;
		}

		return heap_test(repeat, len);
	}

	/* Default values for memcpy test */
	if (len == 0) {
		len = 16 * 1024 * 1024;
	}

	if (repeat == 0) {
		repeat = 64;
	}

	r_len1 = r_len2 = len;

	r_addr1 = malloc(r_len1);
	r_addr2 = malloc(r_len2);

	/* Test memcpy */
	clock_gettime(CLOCK_REALTIME, &time_pre);

	if (nocache) {
		printf("Mark memory as non-cacheable\n");
		flags = PROT_WRITE | PROT_READ | PROT_NOCACHE;
	} else {
		printf("Mark memory as cacheable\n");
		flags = PROT_WRITE | PROT_READ;
	}

	if ((err = vmem_set_flags(vmem_current_context(),
					(mmu_vaddr_t) r_addr1,
					r_len1,
					flags))) {
		printf("Failed to set page attributes! Error %d\n", err);
	}

	if ((err = vmem_set_flags(vmem_current_context(),
					(mmu_vaddr_t) r_addr2,
					r_len2,
					flags))) {
		printf("Failed to set page attributes! Error %d\n", err);
	}

	printf("\n");
	for (int i = 0; i < repeat; i++) {
		memcpy(r_addr1, r_addr2, r_len1);
		printf(".");
		fflush(stdout);
	}
	printf("\n");

	clock_gettime(CLOCK_REALTIME, &time_post);

	seconds = (int) (time_post.tv_sec - time_pre.tv_sec);

	printf("Copied %d bytes in %d seconds (%d bytes/second)\n",
			r_len1 * repeat, seconds, seconds ? r_len1 * repeat / seconds : 0);

	free(r_addr1);
	free(r_addr2);

	return 0;
}
