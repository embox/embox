/**
 * @file
 * @brief High Precision Event Timer (HPET) driver
 *
 * @date 03.04.13
 * @author Roman Kurbatov
 */

#include <stdio.h>
#include <stdint.h>

#include <kernel/time/time_device.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <kernel/printk.h>

#include <embox/unit.h>

#include <acpica/acpi.h>

//ACPI_GENERIC_ADDRESS __attribute__((packed));
//ACPI_TABLE_HPET __attribute__((packed));

static cycle_t hpet_read(void);

static struct time_counter_device hpet_counter_device = {
	.read = &hpet_read
};

static struct clock_source hpet_clock_source = {
	.name = "HPET",
	.event_device = NULL,
	.counter_device = &hpet_counter_device,
	.read = &clock_source_read
};

static ACPI_TABLE_HPET *hpet_table;
static uintptr_t base_address;

EMBOX_UNIT_INIT(hpet_init);

#define GEN_CAP_REG 0x000
#define GEN_CONF_REG 0x010
#define GEN_INT_REG 0x020
#define MAIN_CNT_REG 0x0F0

#define ENABLE_CNF 0x1

#define FEMPTOSEC_IN_SEC 1000000000000000 /* 10^15 */

static inline uint64_t hpet_get_register(uintptr_t offset) {
	return *((uint64_t *) (base_address + offset));
}

static inline void hpet_set_register(uintptr_t offset, uint64_t value) {
	*((uint64_t *) (base_address + offset)) = value;
}

static uint32_t hpet_get_hz() {
	uint64_t reg;
	uint32_t period;

	reg = hpet_get_register(GEN_CAP_REG);
	period = reg >> 32;
	printk("Period: %u\n", period);
	return FEMPTOSEC_IN_SEC / period;
}

static void hpet_start_counter() {
	uint64_t reg;

	reg = hpet_get_register(GEN_CONF_REG);
	reg |= ENABLE_CNF;
	hpet_set_register(GEN_CONF_REG, reg);
}

static int hpet_init(void) {
	char hpet_table_signature[] = "HPET";
	ACPI_STATUS status;

	status = AcpiGetTable(hpet_table_signature,
			1, (ACPI_TABLE_HEADER **) &hpet_table);
	if (ACPI_FAILURE(status)) {
		printk("Unable to get HPET table\n");
		return -1;
	}

	base_address = hpet_table->Address.Address;

	hpet_counter_device.cycle_hz = hpet_get_hz();
	printk("Hz: %u\n", hpet_counter_device.cycle_hz);

	hpet_start_counter();

	for (int i = 0; i < 10; ++i) {
		ksleep(1000);
		printk("%llu\n", (*hpet_clock_source.read)(&hpet_clock_source));
	}

	clock_source_register(&hpet_clock_source);

	return 0;
}

static cycle_t hpet_read(void) {
	return hpet_get_register(MAIN_CNT_REG);
}

