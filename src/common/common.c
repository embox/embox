#include "types.h"
#include "leon.h"
#include "common.h"





void *mem_set (UINT32 *addr, UINT32 patern, UINT32 cnt)
{
	while (cnt --)
		*addr ++= patern;
}

void *memcpy (void *OUT, const void* IN, unsigned int cnt)
{
		BYTE *dest = (BYTE *)OUT;
		BYTE *src = (BYTE *)IN;

        while (cnt--)
            *dest++ = *src++;
}

