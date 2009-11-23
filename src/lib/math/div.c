/**
 * @file div.c
 * @date Aug 26, 2009
 * @author Anton Bondarev, Oleg Medvedev
 */

int __modsi3(int op1, int op2) ;
unsigned int __umodsi3(unsigned int op1, unsigned int op2);
int __divsi3(int op1, int op2);
unsigned int __udivsi3(unsigned int op1, unsigned int op2) ;

int __modsi3(int op1, int op2) {
	int res;
	res = __divsi3(op1,op2);
	return op1 - res * op2;
}

unsigned int __umodsi3(unsigned int op1, unsigned int op2) {
	unsigned int res;
	res = __udivsi3(op1,op2);
	return op1 - res * op2;
}

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
	if (flag1 != flag2)
		result = -result;
	return result;
}

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
		//mask = mask >> 1;
		op2 = op2 >> 1;
	}
	return result;
}
