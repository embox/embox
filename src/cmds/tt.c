/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.04.2013
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <kernel/printk.h>
extern void whereami(void);

extern char **backtrace_symbols(void *const *buff, int size);

void print_symbol(void *addr) {
	void *addresslist[2] = {addr, 0};
	char **symbollist = backtrace_symbols(addresslist, 1);
	printk("%s\n", symbollist[0]);
}


void print_backtrace(void)
{
int topfp;
__asm__ __volatile__ (
"mov %[val], FP" : [val]"=r"(topfp) : :	
);

	for (int i = 0; i < 20; i++) {
		int fp = *(((int*)topfp) - 3);
		int lr = *(((int*)topfp) - 1);
		int pc = *(((int*)topfp) - 0);
		if (i == 0)
			print_symbol((void*) pc); // top frame
		if (fp != -1)
			print_symbol( (void*) lr); // middle frame
		else
			print_symbol( (void*)pc); // bottom frame, lr invalid
		topfp = fp;
	}
}

static void foo() {
	print_backtrace();
}

static void bar() {
	foo();
}

static void baz() {
	bar();
}

int main(int argc, char **argv) {
	//assert(0);
	baz();
	return 0;
}
