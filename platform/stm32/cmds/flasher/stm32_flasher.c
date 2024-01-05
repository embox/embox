/**
 * @file
 *
 * @date   30.10.2020
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <stdio.h>

#include <hal/ipl.h>

static void flasher_copy_blocks(void) {

}

int main(int argc, char **argv) {

	printf("addr flasher(%p)\n", flasher_copy_blocks);

	ipl_disable();
	{

	}
	ipl_enable();

	return 0;
}
