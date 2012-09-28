#include <embox/cmd.h>
#include <stdio.h>
#include <vesa.h>
#include <asm/io.h>

#define	VGA_MISC_WRITE		0x3C2
#define	VGA_MISC_READ		0x3CC

EMBOX_CMD(main);

int main (int argc, char ** argv) {
	unsigned char test = 0;
	out16(VGA_MISC_WRITE, 0xAA);
	test = in16(VGA_MISC_READ);
	printf ("%d :: %d\n", test == 0xAA, test);
	dump_state();
	set_text_mode(argc > 1);
	dump_state();
	demo_graphics();
	dump_state();
	font512();
	dump_state();
	printf ("%d :: %d\n", test == 0xAA, test);
	return 0;
}
