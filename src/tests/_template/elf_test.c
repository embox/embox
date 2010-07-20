/**
 * @file
 * @brief Test function elf_execute.
 *
 * @date 20.07.2010.
 * @author Avdyukhin Dmitry.
 */

#include <embox/test.h>
#include <kernel/elf_executer.h>

EMBOX_TEST(run);

/**
 * Executed file address.
 */
#define FILE_ADDRESS 0x45000000

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int result = 0;

	elf_execute((FILE *)FILE_ADDRESS);

	return result;
}
