/**
 * @file
 *
 * @date Oct 29, 2012
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <hal/arch.h>

int main(int argc, char **argv) {
	arch_shutdown(0);
	return 0;
}
