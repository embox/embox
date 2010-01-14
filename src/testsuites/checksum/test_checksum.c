/**
 * \file test_ram_checksum.c
 *
 * \date 01.07.09
 * \author Alexey Fomin
 */

#include "autoconf.h"
#include "crc32.h"
#include "common.h"
#include "express_tests.h"

DECLARE_EXPRESS_TEST("checksum", exec, TEST_CHECKSUM_ON_BOOT_ENABLE, NULL);

static int exec(int argc, char** argv) {
//	extern unsigned long __checksum;
//	extern unsigned char _text_start, _data_end;
//	unsigned long result = count_md5(&_text_start, &_data_end);
//
//
//	if (result != __checksum) {
//		LOG_ERROR("\n_data_start 0x%x\n", &_text_start);
//		LOG_ERROR("_data_end0x%x\n", &_data_end);
//		LOG_ERROR("__checksum 0x%x\n", __checksum);
//		LOG_ERROR("counting: 0x%x\n", result);
//		return -1;
//	}
//	return 0;
    extern unsigned long __checksum;
    extern unsigned char _trap_table, _text_start, _endtext, _sinittext, _einittext, __express_tests_start, __express_tests_end, __shell_commands_end, _ro_data;
    /*unsigned long result = count_crc32(&_text_start, &_endtext);*/
    unsigned char md5_sum[16];
    const unsigned char *start_addr = &_trap_table;
    const unsigned char *end_addr = &_endtext;

    /*unsigned long result = count_md5(&_text_start, &_endtext);*/
    unsigned int cur_addr;
    int count = 0;
    printf("\n\n");
    for (cur_addr = (unsigned int)start_addr; cur_addr < (unsigned int)end_addr; cur_addr++) {
        //printf("%02x", *(unsigned char *)i);
        count = (count + *(volatile unsigned char *)cur_addr) % (1 << 30);
        if (cur_addr % 20000 == 0) printf("count = %d\n", count);
    }
    printf("count = %d", count);
    printf("\n\n");
    printf("0x%x, 0x%x\n\n", start_addr, end_addr);

//       printf("\n\n");
//       count = 0;
//        for (cur_addr = (unsigned int)start_addr; cur_addr < (unsigned int)end_addr; cur_addr++) {
//            //printf("%02x", *(unsigned char *)i);
//            count = (count + *(volatile unsigned char *)cur_addr) % (1 << 30);
//            if (cur_addr % 20000 == 0) printf("count = %d\n", count);
//        }
//        printf("count = %d", count);
//        printf("\n\n");
//        printf("0x%x, 0x%x\n\n", start_addr, end_addr);

    count_md5(start_addr, end_addr, md5_sum);

    for (cur_addr = 0; cur_addr < 16; cur_addr++){
        printf("%02x", md5_sum[cur_addr]);
    }
    printf("\n\n");
    return 0;
}
