/**
 * @file divsi3.c
 * @date Aug 26, 2009
 * @author Anton Bondarev, Oleg Medvedev
 */

int __divsi3(int op1, int op2) {
	int i;
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
