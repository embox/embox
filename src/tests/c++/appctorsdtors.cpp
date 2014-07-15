/*
 * appctorsdtors.cpp
 *
 *  Created on: 30 janv. 2013
 *      Author: fsulima
 */

#include <stdio.h>

class CtorDtorExample {
protected:
	int field1;
public:
	CtorDtorExample(int i) : field1(i) {
		printf("CtorDtorExample %p,%x constructor.\n", this, field1);
	}
	~CtorDtorExample() {
		printf("CtorDtorExample %p,%x destructor.\n", this, field1);
	}
};

static CtorDtorExample obj1(1);

int main (int argc, char **argv) {
	static CtorDtorExample obj2(2);
	printf("CtorDtorExample main\n");
}
