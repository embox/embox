/**
 * @file
 *
 * @date 26.08.09
 * @author Anton Bondarev, Oleg Medvedev
 */

extern unsigned int __udivsi3(unsigned int op1, unsigned int op2);

unsigned int __umodsi3(unsigned int op1, unsigned int op2) {
	unsigned int res;
	res = __udivsi3(op1,op2);
	return op1 - res * op2;
}
