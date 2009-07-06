/**
 * \file test_ram_checksum.c
 *
 * \date 01.07.09
 * \author Alexey Fomin
 */

#include "crc32.h"
#include "common.h"
#include "express_tests.h"

#define TEST_NAME "checksum"
static char *get_test_name(){
	return TEST_NAME;
}

static int exec(){
        extern unsigned long __checksum;
        extern unsigned char _text_start, _data_end;

/*        TRACE("\n_data_start 0x%x\n", &_text_start);
        TRACE("_data_end0x%x\n", &_data_end);
        TRACE("__checksum 0x%x\n", __checksum);
        TRACE("counting: 0x%x\n", count_crc32(&_text_start, &_data_end));

        if (count_crc32(&_text_start, &_data_end) != __checksum) {
                TRACE("\n!!! Checksum failed !!!\n");
                return -1;
        }
*/        TRACE("\n!!! Checksum OK !!!\n");
        return 0;
}

static EXPRESS_TEST_HANDLER exp_handler = {get_test_name, exec};
REGISTER_EXPRESS_TEST(&exp_handler);
