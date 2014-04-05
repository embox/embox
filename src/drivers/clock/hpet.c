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

static ACPI_TABLE_HEADER *hpet_table;
static void *base_address;

EMBOX_UNIT_INIT(hpet_init);

static void print_bytes(const char *name, const void *p, size_t size) {
	printk("%s bytes: ", name);
	for (size_t i = 0; i < size; ++i) {
		printk("0x%hhx ", *((uint8_t *) p + i));
	}
	printk("\n");
}

static int hpet_init(void) {
	char hpet_table_signature[] = "HPET";
	ACPI_STATUS status;

	status = AcpiGetTable(hpet_table_signature, 1, &hpet_table);
	if (ACPI_FAILURE(status)) {
		printk("Unable to get HPET table\n");
		return -1;
	}

	print_bytes("hpet_table", hpet_table, 56);

	base_address = (void *) (uint32_t) (*(uint64_t *) ((uint8_t *) hpet_table + 44));
	printk("base_address=%p\n", base_address);

	uint64_t resolution = (uint64_t) 1000000000000000 / *((uint32_t *) base_address);
	hpet_counter_device.resolution = resolution;
	printk("Resolution: %llu\n", resolution);

	uint64_t *general_configuration_register = (uint64_t *) ((uint8_t *) base_address + 0x010);
	uint64_t general_configuration_register_value = *general_configuration_register;
	print_bytes("general_configuration_register_value", &general_configuration_register_value, 8);
	general_configuration_register_value |= 0x1;
	print_bytes("general_configuration_register_value", &general_configuration_register_value, 8);
	*general_configuration_register = general_configuration_register_value;

	clock_source_register(&hpet_clock_source);

	for (int i = 0; i < 10; ++i) {
		printk("%lld\n", (*hpet_clock_source.read)(&hpet_clock_source));
		ksleep(1000);
	}

	return 0;
}

static cycle_t hpet_read(void) {
	return *((uint64_t *) ((uint8_t *) base_address + 0x0F0));
}

