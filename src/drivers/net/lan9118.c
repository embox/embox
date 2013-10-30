/**
 * @file
 * @brief Ethernet Controller SMSC LAN9118
 *
 * @date 30.10.13
 * @author Alexander Kalmuk
 */

#include <embox/unit.h>
#include <drivers/gpmc.h>
#include <hal/reg.h>

#define LAN9118_GPMC_CS    5 // GPMC chip-select number
#define SIZE_16M           0x1000000

#define LAN9118_BYTE_TEST  0x64

EMBOX_UNIT_INIT(lan9118_init);

static uint32_t lan9118_base_addr;

static uint32_t lan9181_reg_read(int offset) {
	return REG_LOAD(lan9118_base_addr + offset);
}

static int lan9118_init(void) {
	int res;

	res = gpmc_cs_init(LAN9118_GPMC_CS, &lan9118_base_addr, SIZE_16M);

	if (res < 0)
		return -1;

	if (lan9181_reg_read(LAN9118_BYTE_TEST) != 0x87654321)
		return -1;

	return 0;
}
