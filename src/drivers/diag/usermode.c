/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    01.03.2013
 */

int diag_kbhit(void) {
	return 0;
}

char diag_getc(void) {
	while(1);

	return '\0';
}

void diag_putc(char ch) {
	int ret;
	__asm__ __volatile__(
		"int $0x80"
		: "=a"(ret)
		: "a"(4), "b"(0), "c"(&ch), "d"(1)
		:
	);
}

void diag_init(void) {
}
