/**
 * @file
 * @brief
 *
 * @author  Malkovsky Nikolay
 * @author  Anton Kozlov
 * @date    20.06.2012
 */

#include <util/math.h>

int blog2(int val) {
	static unsigned long log2_magic = 73743071;

	static unsigned char log2_magic_offsets[] = {
	         0,  1,  2,  6,  3,
	        11,  7, 16,  4, 14,
	        12, 21,  8, 23, 17,
	        26, 31,  5, 10, 15,
	        13, 20, 22, 25, 30,
	         9, 19, 24, 29, 18,
	        28, 27
	};

	return log2_magic_offsets[(log2_magic * (val & (~val + 1))) >> 27];
}


