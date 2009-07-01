/**
 * \file test_ram_checksum.c
 *
 * \date 01.07.09
 * \author Alexey Fomin
 */

#include "crc32.h"

int test_checksum() {
	extern long int __checksum;

	if (0 != check_crc32(&_data_start, &_data_end, __checksum))
	{
		TRACE("\n!!! Checksum failed !!!\n");
		return -1;
	}
	return 0;
}
