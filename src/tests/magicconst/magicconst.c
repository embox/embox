/**
 * @file
 * @brief Tests the magic const property: each subinterval
 * 		with length of 5 bits should be distinct.
 *  	Constant = 73743071.
 *      Binary representation 00000100011001010011101011011111
 *
 * @date 25.04.2011
 * @author Malkovsky Nikolay
 */

#include <embox/test.h>

#ifndef MAGIC_CONST_
#define MAGIC_CONST_

static const int _magic_const = 73743071;
static int _bit_num[32];

#define LAST_BIT_NUM_(x) _bit_num[31 & ((_magic_const * (x)) >> 27)]

#endif /* __MAGIC_CONST */

EMBOX_TEST(run);

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int result = 0;

	for(int i = 0; i < 32; ++i) {
		_bit_num[31 & ((_magic_const << i) >> 27)] = i;
	}

	for(int i = 0; i < 32; ++i) {
		if(i != LAST_BIT_NUM_(1 << i)) {
			result = 1;
			break;
		}
	}

    return result;
}
