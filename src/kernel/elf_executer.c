/**
 * @file
 * @brief
 *
 * @date 15.07.2010
 * @author Avdyukhin Dmitry
 *
 */

#include <lib/elf_reader.h>
#include <string.h>
#include <types.h>
#include <errno.h>
#include <stdio.h>

int elf_execute(FILE *file) {
	Elf32_Ehdr *EH;
	Elf32_Phdr *EPH;
	int counter;
	int (*function_main)(int argc, char *argv[]);
	int result;
	EH = (Elf32_Ehdr *)file;
	EPH = (Elf32_Phdr *)((char *)EH + EH->e_phoff);

	counter = EH->e_phnum;
	printf("\n");
	while(counter--) {
		if (EPH->p_type == 1) { /* Type = PT_LOAD. */
			/* Physical address equals to virtual. */
			memcpy((void *)EPH->p_vaddr, (char *)EH + EPH->p_offset, EPH->p_memsz);
		}
		EPH += 1;
	}

	printf("\nData allocated. \nTrying to start at %ld(0x%x)\n", EH->e_entry, EH->e_entry);

	function_main = (int (*)(int argc, char *argv[]))EH->e_entry;
	result = (*function_main) (0, 0);

	printf("\n result : %d\n", result);
	return 0;
}
