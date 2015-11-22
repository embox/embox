/**
 * @file bcm2835_power.c
 *
 * This driver provides the ability to power on and power off hardware, such as
 * the USB Controller, on the BCM2835 SoC used on the Raspberry Pi.  This makes
 * use of the BCM2835's mailbox mechanism.
 *
 * Copyright (c) 2008, Douglas Comer and Dennis Brylow
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted for use in any lawful way, provided that
 * the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the names of the authors nor their contributors may be
 *       used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ''AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS AND CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/types.h>
#include <stdbool.h>
#include <errno.h>

#include <drivers/bcm2835_power.h>
#include <drivers/mailbox/bcm2835_mailbox.h>


#include <embox/unit.h>

EMBOX_UNIT_INIT(bcm2835_power_init);

/* Retrieve the bitmask of power on/off state. */
static uint32_t bcm2835_get_power_mask(void) {
	return (bcm2835_mailbox_read(BCM2835_POWER_MGMT_CHANNEL) >> 4);
}

/* Set the bitmask of power on/off state. */
static void bcm2835_set_power_mask(uint32_t mask) {
	bcm2835_mailbox_write(mask << 4, BCM2835_POWER_MGMT_CHANNEL);
}

/* Bitmask that gives the current on/off state of the BCM2835 hardware.
 * This is a cached value. */
static uint32_t bcm2835_power_mask;

/**
 * Powers on or powers off BCM2835 hardware.
 *
 * @param feature
 *      Device or hardware to power on or off.
 * @param on
 *      ::TRUE to power on; ::FALSE to power off.
 *
 * @return
 *      0 if successful; -EIO otherwise.
 */
int bcm2835_setpower(enum board_power_feature feature, bool on) {
	uint32_t bit;
	uint32_t newmask;
	bool is_on;

	bit = 1 << feature;
	is_on = (bcm2835_power_mask & bit) != 0;
	if (on != is_on) {
		newmask = bcm2835_power_mask ^ bit;
		bcm2835_set_power_mask(newmask);
		bcm2835_power_mask = bcm2835_get_power_mask();
		if (bcm2835_power_mask != newmask) {
			return -EIO;
		}
	}

	return 0;
}

/**
 * Resets BCM2835 power to default state (all devices powered off).
 */
static int bcm2835_power_init(void) {
	bcm2835_set_power_mask(0);
	bcm2835_power_mask = bcm2835_get_power_mask();

	return 0;
}
