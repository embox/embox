/**
 * @file
 * @brief EFM32ZG-STK3200 LCD driver.
 *
 * @author  Roman Kurbatov
 * @date    05.04.2014
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <embox/unit.h>

#include <display.h>


EMBOX_UNIT_INIT(efm_lcd_init);

#define START_WITH_LOGO OPTION_GET(NUMBER, start_with_logo)

#define DISPLAY_DEVICE_NO (0)

extern const uint8_t demo_image_mono_128x128[128][16];

static int efm_lcd_init(void) {
	DISPLAY_Device_t      displayDevice;
	EMSTATUS status;
	DISPLAY_PixelMatrix_t pixelMatrixBuffer;

	/* Initialize the DISPLAY module. */
	status = DISPLAY_Init();
	if (DISPLAY_EMSTATUS_OK != status) {
		return status;
	}

	/* Retrieve the properties of the DISPLAY. */
	status = DISPLAY_DeviceGet(DISPLAY_DEVICE_NO, &displayDevice);
	if (DISPLAY_EMSTATUS_OK != status) {
		return status;
	}
	/* Allocate a framebuffer from the DISPLAY device driver. */
	displayDevice.pPixelMatrixAllocate(&displayDevice,
			displayDevice.geometry.width,
			displayDevice.geometry.height,
			&pixelMatrixBuffer);
#if START_WITH_LOGO
	memcpy(pixelMatrixBuffer, demo_image_mono_128x128,
			displayDevice.geometry.width * displayDevice.geometry.height / 8 );

	status = displayDevice.pPixelMatrixDraw(&displayDevice,
			pixelMatrixBuffer,
			0,
			displayDevice.geometry.width,
			0,
			displayDevice.geometry.height);
#endif
	return 0;
}
