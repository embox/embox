/**
 * @file
 *
 * @date 26.08.09
 * @author Anton Bondarev, Oleg Medvedev
 */

extern int __divsi3(int op1, int op2);

int __modsi3(int op1, int op2) {
	int res;
	res = __divsi3(op1,op2);
	return op1 - res * op2;
}
