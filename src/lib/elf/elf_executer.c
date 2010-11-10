/**
 * @file
 * @brief ELF object file execute
 *
 * @date 15.07.2010
 * @author Avdyukhin Dmitry
 */

#include <lib/libelf.h>
#include <string.h>
#include <types.h>
#include <errno.h>
#include <stdio.h>
#include <hal/ipl.h>

int elf_execve(unsigned long *file_addr, char *argv[]) {
	int (*function_main)(int argc, char * argv[]);
	int result, counter;
	Elf32_Ehdr *EH;
	Elf32_Phdr *EPH;
	ipl_t ipl;

	EH = (Elf32_Ehdr *)file_addr;

	if (EH->e_ident[EI_MAG0] != ELFMAG0 ||
	    EH->e_ident[EI_MAG1] != ELFMAG1 ||
	    EH->e_ident[EI_MAG2] != ELFMAG2 ||
	    EH->e_ident[EI_MAG3] != ELFMAG3) {
		TRACE("not an ELF file\n");
		return -1;
	}

	EPH = (Elf32_Phdr *)((char *)EH + EH->e_phoff);

	ipl = ipl_save();
	counter = EH->e_phnum;
	while(counter--) {
		if (EPH->p_type == PT_LOAD) {
			/* Physical address equals to virtual. */
			memcpy((void *)EPH->p_paddr, (char *)EH + EPH->p_offset, EPH->p_memsz);
		}
		EPH = (Elf32_Phdr *)((unsigned char *)EPH + EH->e_phentsize);
	}

	TRACE("Data allocated.\n");
	TRACE("Trying to start at %ld(0x%x)\n\n\n", (long)EH->e_entry, (uint32_t)EH->e_entry);

	function_main = (int (*)(int argc, char *argv[]))EH->e_entry;
	result = function_main (0, argv);
	ipl_restore(ipl);

	TRACE("\n result : %d\n", result);
	return result;
}
