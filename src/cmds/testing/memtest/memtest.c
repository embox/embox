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
#include <unistd.h>
#include <time.h>

#include <mem/vmem.h>

static const size_t default_region_len = 16 * 1024 * 1024;

static void print_help(char **argv) {
	assert(argv);
	assert(argv[0]);
	printf("Usage: %s [-nh] [-l LENGHT]\n", argv[0]);
	printf("\tOptions: -n Test non-cached memory\n");
	printf("\t         -l LENGTH Number of bytes to be copied (default 0x1000000)\n");
	printf("\t         -r REPEAT Number of repeats (default 64)\n");
}

int main(int argc, char **argv) {
	int opt, nocache = 0, err;
	size_t len = 0;
	int seconds;
	int repeat = 64;
	vmem_page_flags_t flags;

	struct timespec time_post;
	struct timespec time_pre;

	void *r_addr1, *r_addr2;
	size_t r_len1, r_len2;

	while (-1 != (opt = getopt(argc, argv, "nhl:r:"))) {
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
		case 'h':
			print_help(argv);
			return 0;
		}
	}

	if (len == 0) {
		len = default_region_len;
	}

	r_len1 = r_len2 = len;

	r_addr1 = malloc(r_len1);
	r_addr2 = malloc(r_len2);

	/* Test memcpy */
	clock_gettime(CLOCK_REALTIME, &time_pre);

	if (nocache) {
		printf("Mark memory as non-cacheable\n");
		flags = VMEM_PAGE_WRITABLE;
	} else {
		printf("Mark memory as cacheable\n");
		flags = VMEM_PAGE_WRITABLE | VMEM_PAGE_CACHEABLE;
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
