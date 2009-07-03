/**
 * \file test_ram_checksum.c
 *
 * \date 01.07.09
 * \author Alexey Fomin
 */

#include "crc32.h"

int test_checksum() {
	extern unsigned long __checksum;
	extern unsigned char _data_start, _data_end;

	if (count_crc32(&_data_start, &_data_end) != __checksum)
	{
		TRACE("\n!!! Checksum failed !!!\n");
		return -1;
	}
	TRACE("\n!!! Checksum OK !!!\n");
	return 0;
}
