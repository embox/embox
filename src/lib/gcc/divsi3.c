/**
 * @file
 *
 * @date 26.08.09
 * @author Anton Bondarev, Oleg Medvedev
 */

extern unsigned int __udivsi3(unsigned int op1, unsigned int op2);

int __divsi3(int op1, int op2) {
	int flag1, flag2;
	int result;
	flag1 = op1 < 0 ? 1 : 0;
	flag2 = op2 < 0 ? 1 : 0;

	if (flag1) {
		op1 = -op1;
	}

	if (flag2) {
		op2 = -op2;
	}
	result = __udivsi3(op1, op2);
	return (flag1 != flag2) ? -result : result;
}
