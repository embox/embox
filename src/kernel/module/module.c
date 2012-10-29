/**
 * @file
 * @brief
 *
 * @date 26.10.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <string.h>
#include <stdlib.h>

#include <lib/libelf.h>

EMBOX_UNIT(unit_init, unit_fini);

extern char _symtab_start, _symtab_end;
extern char _strtab_start;

static Elf32_Obj kernel;

static int unit_init() {
	/* Fills all necessary information of the kernel. */
	kernel.header = malloc(sizeof(Elf32_Ehdr));
	kernel.header->e_type = ET_EXEC;

	kernel.sym_table = (Elf32_Sym *) &_symtab_start;
	kernel.sym_count = ((size_t) &_symtab_end - (size_t) &_symtab_start) / sizeof(Elf32_Sym);
	kernel.sym_names = &_strtab_start;

	return 0;
}

static int unit_fini() {
	free(kernel.header);

	return 0;
}

Elf32_Addr find_symbol_addr(const char *name) {
	Elf32_Sym *sym;

	for (int i = 0; i < kernel.sym_count; i++) {
		sym = &kernel.sym_table[i];
		// FIXME:
		if ((ELF32_ST_BIND(sym->st_info) == STB_GLOBAL)
			&& (0 == strcmp(name, kernel.sym_names + sym->st_name))) {
			// FIXME: if relocatable object - another address
			return sym->st_value;
		}
	}

	return 0;
}

