/**
 * @file
 * @brief testing struct and pointer.
 *
 * @date 4.05.2010
 * @autor Darya Dzendzik
 */

#include <stdio.h>
typedef struct tr {
	void (* pfunc)();
	} tr_t;

void func1(){
	printf("!");
	}
void func2(){
	printf("@");
	}
void func3(){
	printf("#");
	}
void func4(){
	printf("$");
	}
void func5(){
	printf("\\");
	}
void func6(){
	printf("^");
	}
void func7(){
	printf("&");
	}
void func8(){
	printf("*");
	}
void func9(){
	printf("=");
	}
void func0(){
	printf("+");
	}

int main(){
        int i, j;

	tr_t trn[10];

	trn[0].pfunc = func0;
	trn[1].pfunc = func1;
	trn[2].pfunc = func2;
	trn[3].pfunc = func3;
	trn[4].pfunc = func4;
	trn[5].pfunc = func5;
	trn[6].pfunc = func6;
	trn[7].pfunc = func7;
	trn[8].pfunc = func8;
	trn[9].pfunc = func9;

	for ( i = 0; i < 10; i++) {
		for ( j = 0; j < 100; j++) {
			trn[i].pfunc();
		}
		printf("\n");
	}

	return 0;
}
