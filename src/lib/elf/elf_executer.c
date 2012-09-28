/**
 * @file
 * @brief ELF object file execute
 *
 * @date 15.07.10
 * @author Avdyukhin Dmitry
 */

#include <lib/libelf.h>
#include <string.h>
#include <types.h>
#include <errno.h>
#include <stdio.h>

static inline int32_t elf_read_segment(FILE *fd, Elf32_Ehdr *head, Elf32_Phdr *EPH, int8_t *dst) {
	uint8_t rev = head->e_ident[EI_DATA];
	size_t size;
	long offset;

	offset = L_REV(EPH->p_offset, rev);
	size = L_REV(EPH->p_memsz, rev);

	if (size) {
		fseek(fd, offset, 0);
		return (1 == fread(dst, size, 1, fd)) ? 1 : -1;
	} else {
		/* Empty segment. */
		return 0;
	}
}

int elf_execve(FILE *fd, Elf32_Ehdr *EH, Elf32_Phdr *EPH) {
	int (*function_main)(int argc, char * argv[]);
	int result, counter;

	counter = EH->e_phnum;
	while (counter--) {
		if (EPH->p_type == PT_LOAD) {
			/* Physical address equals to virtual. */
			elf_read_segment(fd, EH, EPH, (int8_t *)EPH->p_paddr);
		}
		EPH = (Elf32_Phdr *) ((unsigned char *) EPH + EH->e_phentsize);
	}

	printf("Data allocated.\n");
	printf("Trying to start at 0x%x\n", (uint32_t)EH->e_entry);

    function_main = (int (*)(int argc, char *argv[])) EH->e_entry;
    result = function_main (0, NULL);

	printf("result : %d", result);
	return 0;
}
