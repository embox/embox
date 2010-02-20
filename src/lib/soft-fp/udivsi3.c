/**
 * @file
 *
 * @date 26.08.2009
 * @author Anton Bondarev, Oleg Medvedev
 */

unsigned int __udivsi3(unsigned int op1, unsigned int op2) {
	int i;
	unsigned int result = 0;
	int steps = 0;
	if (0 == op2) {
		return 0;
	}

	while (0x80000000 != (op2 & 0x80000000)) {
		op2 = op2 << 1;
		steps++;
	}

	for (i = 0; i <= steps; i++) {
		result = result << 1;
		if (op1 >= op2) {
			result += 1;
			op1 -= op2;
		}
		op2 = op2 >> 1;
	}
	return result;
}
