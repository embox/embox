/**
 * @file
 * @brief
 *
 * @date 14.11.12
 * @author Anton Bulychev
 */

#include <lib/libelf.h>

Elf32_Addr elf_get_base_address(Elf32_Obj *obj) {
	Elf32_Addr base_address = 0xFFFFFFFF;
	Elf32_Phdr *ph_table = obj->ph_table;

	for (int i = 0; i < obj->header->e_phnum; i++) {
		if ((ph_table[i].p_type == PT_LOAD)
		&& (ph_table[i].p_vaddr < base_address)) {
			base_address = ph_table[i].p_vaddr;
		}
	}

	return base_address & 0x1000; // PAGE SIZE
}
