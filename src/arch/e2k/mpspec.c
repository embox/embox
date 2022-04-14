/**
 * @file
 *
 * @date Apr 11, 2022
 * @author Anton Bondarev
 */

#include <stdint.h>


#include <kernel/printk.h>

#include <e2k_api.h>
#include <e2k_mas.h>

#include <asm/bootinfo.h>
#include <asm/mpspec.h>

static uintptr_t clock_base = 0;
static uintptr_t uart_base = 0;
static uintptr_t pci_conf_base = -1;

int mpspec_init(void) {
	uintptr_t info;
	uintptr_t mp_table;
	uint32_t float_sig, conf_sig;
	uint8_t cpu_type;
	uint_fast16_t entry_num;

	info = (uintptr_t)E2K_READ_MAS_D(&BOOTBLOCK.info.mp_table_base, MAS_MODE_LOAD_PA);

	float_sig = E2K_READ_MAS_W(&((struct intel_mp_floating *)info)->mpf_signature, MAS_MODE_LOAD_PA);

	mp_table = (uintptr_t)E2K_READ_MAS_D(&((struct intel_mp_floating *)info)->mpf_physptr, MAS_MODE_LOAD_PA);
	conf_sig = E2K_READ_MAS_W(&((struct mp_config_table *)mp_table)->mpc_signature, MAS_MODE_LOAD_PA);

	if (float_sig != SMP_FLOATING_TABLE_SIG) {
		printk(" Error: mpf_signature is %x (need %x), ", float_sig, SMP_FLOATING_TABLE_SIG);
		return -1;
	}

	if (conf_sig != SMP_CONFIG_TABLE_SIG) {
		printk(" Error: mpc_signature is %x (need %x), ", conf_sig, SMP_CONFIG_TABLE_SIG);
		return -1;
	}

	entry_num = E2K_READ_MAS_H(&((struct mp_config_table *)mp_table)->mpc_oemcount, MAS_MODE_LOAD_PA);
	mp_table += sizeof (struct mp_config_table);
	for (uint_fast16_t i = 0; i < entry_num; ++i) {
		switch (E2K_READ_MAS_B(mp_table, MAS_MODE_LOAD_PA)) {
		case MP_PROCESSOR:
			mp_table += sizeof (struct mpc_config_processor);
			continue;
		case MP_BUS:
			mp_table += sizeof (struct mpc_config_bus);
			continue;
		case MP_IOAPIC:
			mp_table += sizeof (struct mpc_config_ioapic);
			continue;
		case MP_INTSRC:
			mp_table += sizeof (struct mpc_config_intsrc);
			continue;
		case MP_LINTSRC:
			mp_table += sizeof (struct mpc_config_lintsrc);
			continue;
		case MP_TIMER:
			clock_base = E2K_READ_MAS_D(&((struct mpc_config_timer *)mp_table)->mpc_timeraddr, MAS_MODE_LOAD_PA);
			if (clock_base == 0 || clock_base > UINT32_MAX) {
				printk(" Error: unsupported mpc_timeraddr value, ");
				return -1;
			}
			mp_table += sizeof (struct mpc_config_timer);
			continue;
		case MP_I2C_SPI:
			mp_table += sizeof (struct mpc_config_i2c);
			continue;
		case MP_IOLINK:
			mp_table += sizeof (struct mpc_config_iolink);
			continue;
		case MP_PMC:
			mp_table += sizeof (struct mpc_config_pmc);
			continue;
		default:
			mp_table += E2K_READ_MAS_B(&((struct mp_exten_config *)mp_table)->mpe_length, MAS_MODE_LOAD_PA);
			continue;
		}
		break;
	}

	uart_base = ((uintptr_t)E2K_READ_MAS_W(&BOOTBLOCK.info.serial_base, MAS_MODE_LOAD_PA));

	cpu_type = E2K_READ_MAS_B(&BOOTBLOCK.info.bios.cpu_type, MAS_MODE_LOAD_PA);
	switch (cpu_type) {
	case CPU_TYPE_E1CP:
		pci_conf_base = 0xff10000000UL;
		break;
	case CPU_TYPE_E3M:
		pci_conf_base = 0;
		break;
	case CPU_TYPE_E2S:
	case CPU_TYPE_E8C:
	case CPU_TYPE_E8C2:
		/* TODO Get SIC_RT_PCICFG_BASE */
		pci_conf_base = 0x200000000;
		break;
	default:
		pci_conf_base = -1;
	}
//	printk("cpu_type(0x%x) pci_conf_base (0x%lx)\n", cpu_type, pci_conf_base);
	return 0;
}

uint64_t mpspec_get_clock_base(void) {
	return clock_base;
}

uintptr_t mpspec_get_uart_base(void) {
	return uart_base;
}

uint64_t mpspec_get_pci_conf_base(void) {
	return pci_conf_base;
}
