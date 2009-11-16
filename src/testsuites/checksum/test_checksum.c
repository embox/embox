/**
 * \file test_ram_checksum.c
 *
 * \date 01.07.09
 * \author Alexey Fomin
 */

#include "crc32.h"
#include "common.h"
#include "express_tests.h"

DECLARE_EXPRESS_TEST("checksum", exec);

static int exec() {
	extern unsigned long __checksum;
	extern unsigned char _text_start, _data_end;
	unsigned long result = count_crc32(&_text_start, &_data_end);

	if (result != __checksum) {
		LOG_ERROR("\n_data_start 0x%x\n", &_text_start);
		LOG_ERROR("_data_end0x%x\n", &_data_end);
		LOG_ERROR("__checksum 0x%x\n", __checksum);
		LOG_ERROR("counting: 0x%x\n", result);
		return -1;
	}
	return 0;
}
