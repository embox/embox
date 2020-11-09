/**
 * @file
 * @brief EFM32ZG-STK3200 LCD driver.
 *
 * @author  Roman Kurbatov
 * @date    05.04.2014
 */

#include <stdlib.h>

#include <embox/unit.h>

#include <displayls013b7dh03.h>

EMBOX_UNIT_INIT(init);

static int init(void) {
	DISPLAY_Ls013b7dh03Init();

	return 0;
}
