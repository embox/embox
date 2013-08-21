/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.08.2013
 */

extern int __mulsi3(int op1, int op2);

int __mulhi3(int op1, int op2) {
	return __mulsi3(op1, op2);
}

