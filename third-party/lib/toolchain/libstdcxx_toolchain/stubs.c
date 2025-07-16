/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.05.25
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int getentropy(void *buffer, size_t length) {
	fprintf(stderr, ">>>%s\n", __func__);
	return -1;
}

uint32_t arc4random(void) {
	fprintf(stderr, ">>>%s\n", __func__);
	return 0;
}
