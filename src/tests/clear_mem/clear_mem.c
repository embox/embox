/**
 * @file
 *
 * @date 19.02.10
 * @author Alexey Fomin
 */

#include <embox/test.h>
#include <string.h>

EMBOX_TEST(run);

#define BASE_ADDR 0x40000000
#define END_ADDR  0x44000000

static int run(void) {
	memset((void *)BASE_ADDR, 0, (END_ADDR - BASE_ADDR) / sizeof(char));
	return 0;
}
