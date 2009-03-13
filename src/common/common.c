#include "types.h"
#include "leon.h"
#include "common.h"

void *mem_set(UINT32 *addr, UINT32 patern, UINT32 cnt) {
	while (cnt--)
		*addr++ = patern;
}

void *memcpy(void *destination, const void* source, unsigned int cnt) {
	BYTE *dest = (BYTE *) destination;
	BYTE *src = (BYTE *) source;

	while (cnt--)
		*dest++ = *src++;

	return destination;
}

