#include <stdio.h>

extern void static_square(void);

int main(int argc, char *argv[]) {
	static_square();
	return 0;
}
