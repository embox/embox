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
#include <stdbool.h>

#include <kernel/spinlock.h>

static spinlock_t atomic_lock = SPIN_STATIC_UNLOCKED;

uint8_t __atomic_fetch_sub_1(void *mem, uint8_t val, int model) {
	spin_lock(&atomic_lock);
	uint8_t ret = *((uint8_t *) mem);
	*((uint8_t *) mem) -= val;
	spin_unlock(&atomic_lock);
	return ret;
}

uint16_t __atomic_fetch_sub_2(void *mem, uint16_t val, int model) {
	spin_lock(&atomic_lock);
	uint16_t ret = *((uint16_t *) mem);
	*((uint16_t *) mem) -= val;
	spin_unlock(&atomic_lock);
	return ret;
}

uint32_t __atomic_fetch_sub_4(void *mem, uint32_t val, int model) {
	spin_lock(&atomic_lock);
	uint32_t ret = *((uint32_t *) mem);
	*((uint32_t *) mem) -= val;
	spin_unlock(&atomic_lock);
	return ret;
}

uint64_t __atomic_fetch_sub_8(void *mem, uint64_t val, int model) {
	spin_lock(&atomic_lock);
	uint64_t ret = *((uint64_t *) mem);
	*((uint64_t *) mem) -= val;
	spin_unlock(&atomic_lock);
	return ret;
}

uint8_t __atomic_fetch_add_1(void *mem, uint8_t val, int model) {
	spin_lock(&atomic_lock);
	uint8_t ret = *((uint8_t *) mem);
	*((uint8_t *) mem) += val;
	spin_unlock(&atomic_lock);
	return ret;
}

uint16_t __atomic_fetch_add_2(void *mem, uint16_t val, int model) {
	spin_lock(&atomic_lock);
	uint16_t ret = *((uint16_t *) mem);
	*((uint16_t *) mem) += val;
	spin_unlock(&atomic_lock);
	return ret;
}

uint32_t __atomic_fetch_add_4(void *mem, uint32_t val, int model) {
	spin_lock(&atomic_lock);
	uint32_t ret = *((uint32_t *) mem);
	*((uint32_t *) mem) += val;
	spin_unlock(&atomic_lock);
	return ret;
}

uint64_t __atomic_fetch_add_8(void *mem, uint64_t val, int model) {
	spin_lock(&atomic_lock);
	uint64_t ret = *((uint64_t *) mem);
	*((uint64_t *) mem) += val;
	spin_unlock(&atomic_lock);
	return ret;
}

#ifndef __clang__
uint8_t __sync_val_compare_and_swap_1(volatile void *ptr, uint8_t soldval,
		uint8_t snewval) {
	spin_lock(&atomic_lock);
	uint8_t ret = *((uint8_t *) ptr);
	*((uint8_t *) ptr) = snewval;
	spin_unlock(&atomic_lock);
	return ret;
}

uint16_t __sync_val_compare_and_swap_2(volatile void *ptr, uint16_t soldval,
		uint16_t snewval) {
	spin_lock(&atomic_lock);
	uint16_t ret = *((uint16_t *) ptr);
	*((uint16_t *) ptr) = snewval;
	spin_unlock(&atomic_lock);
	return ret;
}

uint32_t __sync_val_compare_and_swap_4(volatile void *ptr, uint32_t soldval,
		uint32_t snewval) {
	spin_lock(&atomic_lock);
	uint32_t ret = *((uint32_t *) ptr);
	*((uint32_t *) ptr) = snewval;
	spin_unlock(&atomic_lock);
	return ret;
}

uint64_t __sync_val_compare_and_swap_8(volatile void *ptr, uint64_t soldval,
		uint64_t snewval) {
	spin_lock(&atomic_lock);
	uint64_t ret = *((uint64_t *) ptr);
	*((uint64_t *) ptr) = snewval;
	spin_unlock(&atomic_lock);
	return ret;
}

void __sync_synchronize(void) {
}

uint32_t __sync_add_and_fetch_4(void *mem, uint32_t val) {
	return __atomic_fetch_add_4(mem, val, 0) + val;
}

uint32_t __sync_sub_and_fetch_4(void *mem, uint32_t val) {
	return __atomic_fetch_sub_8(mem, val, 0) - val;
}

uint32_t __sync_fetch_and_add_4(void *mem, uint32_t val) {
	return __atomic_fetch_add_4(mem, val, 0);
}

uint32_t __sync_fetch_and_sub_4(void *mem, uint32_t val) {
	return __atomic_fetch_sub_4(mem, val, 0);
}
#endif /* !__clang__ */

unsigned char __atomic_exchange_1(volatile void *p, unsigned char val, int memorder) {
	unsigned char prev;

	(void) memorder;

	spin_lock(&atomic_lock);
	prev = *(unsigned char *) p;
	*(unsigned char *) p = val;
	spin_unlock(&atomic_lock);

	return prev;
}

bool __atomic_compare_exchange_4(volatile void *p, void *e,
	uint32_t desired, bool weak,
	int success_memorder, int failure_memorder) {
	(void) success_memorder;
	(void) failure_memorder;
	(void) weak;
	bool ret;
	uint32_t *ptr;
	uint32_t *expected;


	ptr = (uint32_t *)p;
	expected = e;

	spin_lock(&atomic_lock);
	if (*ptr == *expected) {
		*ptr = desired;
		ret = true;
	} else {
		*expected = *ptr;
		ret = false;
	}
	spin_unlock(&atomic_lock);
	return ret;
}

uint64_t __atomic_load_8(volatile void *mem, int model) {
	spin_lock(&atomic_lock);
	uint64_t ret = *((uint64_t *) mem);
	spin_unlock(&atomic_lock);
	return ret;
}

void __atomic_store_8(volatile void *mem, uint64_t val, int model) {
	spin_lock(&atomic_lock);
	*((uint64_t *) mem) = val;
	spin_unlock(&atomic_lock);
	return;
}
