/**
 * @file pseudo-atomic.c
 * @brief This file contains functions for successful compilation
 * of cpp source for ARM platform which relies on atomic operations
 * which are not provided by arm-none-eabi-g++ for some reason.
 * These functions are not atomic and are used just to make programs
 * compiled. IT IS STRONGLY RECOMMENDED NOT TO USE THIS MODULE.
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 11.08.2017
 */

#include <stdint.h>

uint8_t __atomic_fetch_sub_1(void *mem, uint8_t val, int model) {
	uint8_t ret = *((uint8_t *) mem);
	*((uint8_t *) mem) -= val;
	return ret;
}

uint16_t __atomic_fetch_sub_2(void *mem, uint16_t val, int model) {
	uint16_t ret = *((uint16_t *) mem);
	*((uint16_t *) mem) -= val;
	return ret;
}

uint32_t __atomic_fetch_sub_4(void *mem, uint32_t val, int model) {
	uint32_t ret = *((uint32_t *) mem);
	*((uint32_t *) mem) -= val;
	return ret;
}

uint64_t __atomic_fetch_sub_8(void *mem, uint64_t val, int model) {
	uint64_t ret = *((uint64_t *) mem);
	*((uint64_t *) mem) -= val;
	return ret;
}

uint8_t __atomic_fetch_add_1(void *mem, uint8_t val, int model) {
	uint8_t ret = *((uint8_t *) mem);
	*((uint8_t *) mem) += val;
	return ret;
}

uint16_t __atomic_fetch_add_2(void *mem, uint16_t val, int model) {
	uint16_t ret = *((uint16_t *) mem);
	*((uint16_t *) mem) += val;
	return ret;
}

uint32_t __atomic_fetch_add_4(void *mem, uint32_t val, int model) {
	uint32_t ret = *((uint32_t *) mem);
	*((uint32_t *) mem) += val;
	return ret;
}

uint64_t __atomic_fetch_add_8(void *mem, uint64_t val, int model) {
	uint64_t ret = *((uint64_t *) mem);
	*((uint64_t *) mem) += val;
	return ret;
}

uint8_t __sync_val_compare_and_swap_1(uint8_t *ptr, uint8_t soldval,
		uint8_t snewval) {
	uint8_t ret = *ptr;
	*ptr = snewval;
	return ret;
}

uint16_t __sync_val_compare_and_swap_2(uint16_t *ptr, uint16_t soldval,
		uint16_t snewval) {
	uint16_t ret = *ptr;
	*ptr = snewval;
	return ret;
}

uint32_t __sync_val_compare_and_swap_4(uint32_t *ptr, uint32_t soldval,
		uint32_t snewval) {
	uint32_t ret = *ptr;
	*ptr = snewval;
	return ret;
}

uint64_t __sync_val_compare_and_swap_8(uint64_t *ptr, uint64_t soldval,
		uint64_t snewval) {
	uint64_t ret = *ptr;
	*ptr = snewval;
	return ret;
}

uint32_t __aeabi_atexit(void *obj, void *func, void *d) {
	return 0;
}

void __sync_synchronize(void) {
}
