/**
 * @file
 * @brief Test function elf_execve.
 *
 * @date 20.07.2010.
 * @author Avdyukhin Dmitry.
 */

#include <embox/test.h>
#include <lib/libelf.h>

EMBOX_TEST(run);

/**
 * Executed file address.
 */
#define FILE_ADDRESS 0x46000000

static int run(void) {
	return elf_execve((unsigned long *)FILE_ADDRESS, NULL);
}
