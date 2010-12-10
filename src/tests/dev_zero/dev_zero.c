/**
 * @file
 * @brief This file is derived from EMBOX test template.
 * @description simple test for Zero device
 *
 * @author Fedor Burdun
 * @date 12.07.2010
 */

#include <embox/test.h>
#include <kernel/driver.h>
#include <stdio.h>

EMBOX_TEST(run);

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int result = 0;
	int i;

	printf("\e[1;34mSimple test for \e[1;31m /dev/zero \e[0;0m\n");

	FILE dev_zero;
	if (!(dev_zero = device_select("dev_zero"))) {
		printf("\nCouldn't open /dev/zero. Check mods-device and include embox.drive.zero if isn't.\n");
		return 0;	 /* return 1 */
	}
	printf("device id in system: %d\n",dev_zero);
	printf("try read 10 chars from /dev/zero: ");
	for (i = 0; i < 10; ++i) {
		putchar(fgetc(dev_zero));
	}
	printf("\ntry write 10 chars to /dev/zero: ");
	for (i = 0; i < 10; ++i) {
		fputc(dev_zero, 'c');
	}

	printf("\n\e[1;31mtest done. \e[0;0m");

	return result;
}
