/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    01.08.2013
 */

int __mulsi3(int a, int b) {
	int res = 0;

	while (b) {
		if (a & 1)
			res += b;
		a >>= 1;
		b <<= 1;
	}

	return res;
}


