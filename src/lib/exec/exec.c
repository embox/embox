/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <lib/elfloader.h>
#include <kernel/usermode.h>
#include <mem/mmap.h>
#include <kernel/task.h>

#define AT_NULL		0		/* End of vector */
#define AT_IGNORE	1		/* Entry should be ignored */
#define AT_EXECFD	2		/* File descriptor of program */
#define AT_PHDR		3		/* Program headers for program */
#define AT_PHENT	4		/* Size of program header entry */
#define AT_PHNUM	5		/* Number of program headers */
#define AT_PAGESZ	6		/* System page size */
#define AT_BASE		7		/* Base address of interpreter */
#define AT_FLAGS	8		/* Flags */
#define AT_ENTRY	9		/* Entry point of program */
#define AT_NOTELF	10		/* Program is not ELF */
#define AT_UID		11		/* Real uid */
#define AT_EUID		12		/* Effective uid */
#define AT_GID		13		/* Real gid */
#define AT_EGID		14		/* Effective gid */
#define AT_CLKTCK	17		/* Frequency of times() */

typedef struct {
	uint32_t entry;
	uint32_t interp_entry;
	uint32_t base_addr;
	uint32_t phdr;
	uint32_t phnum;
	uint32_t phent;
	char *filename;
} exec_t;


static inline uint32_t stack_push_str(uint32_t *stack, const char *str) {
	*stack -= strlen(str) + 1;
	memcpy((void *) (*stack), str, strlen(str) + 1);
	return *stack;
}

static inline uint32_t stack_push_int(uint32_t *stack, uint32_t val) {
	*stack -= 4;
	memcpy((void *)(*stack), &val, 4);
	return *stack;
}

static void stack_push_aux(uint32_t *stack, uint32_t type, uint32_t val) {
	stack_push_int(stack, val);
	stack_push_int(stack, type);
}

static void fill_stack(uint32_t *stack, exec_t *exec) {
	uint32_t p;

	// end marker
	stack_push_int(stack, 0);

	// argv[0]
	p = stack_push_str(stack, exec->filename);

	// padding
	stack_push_int(stack, 0);

	stack_push_aux(stack, AT_NULL, 0);
	stack_push_aux(stack, AT_PAGESZ, 0x1000);

	if (exec->base_addr) {
		stack_push_aux(stack, AT_BASE, exec->base_addr);
	}

	stack_push_aux(stack, AT_ENTRY, exec->entry);

	stack_push_aux(stack, AT_PHDR, exec->phdr);
	stack_push_aux(stack, AT_PHNUM, exec->phnum);
	stack_push_aux(stack, AT_PHENT, exec->phent);

	// What is it?
	stack_push_aux(stack, AT_UID, 1000);
	stack_push_aux(stack, AT_EUID, 1000);
	stack_push_aux(stack, AT_GID, 1000);
	stack_push_aux(stack, AT_EGID, 1000);

	// envp[]
	stack_push_int(stack, 0);

	// argv[]
	stack_push_int(stack, 0);
	stack_push_int(stack, p);

	// argc
	stack_push_int(stack, 1);
}

static int load_interp(char *filename, exec_t *exec) {
	Elf32_Ehdr header;
	Elf32_Phdr *ph_table, *ph;
	Elf32_Addr base_addr;
	struct marea *marea;
	size_t size;
	int err;
	int fd = open(filename, O_RDONLY);

	if (fd == -1) {
		return -EBADF;
	}

	if ((err = elf_read_header(fd, &header))) {
		return err;
	}

	if (header.e_type != ET_DYN) {
		return -EBADF;
	}

	size = header.e_phnum * header.e_phentsize;

	if (!(ph_table = malloc(size))) {
		return -ENOMEM;
	}
	elf_read_ph_table(fd, &header, ph_table);


	size = 0;
	for (int i = 0; i < header.e_phnum; i++) {
		ph = &ph_table[i];

		if ((ph->p_type == PT_LOAD) && (size < ph->p_vaddr + ph->p_memsz)) {
			size = ph->p_vaddr + ph->p_memsz;
		}
	}

	if (!(marea = mmap_alloc_marea(task_self()->mmap, size, 0))) {
		free(ph_table);
		return -ENOMEM;
	}

	base_addr = marea_get_start(marea);

	for (int i = 0; i < header.e_phnum; i++) {
		ph = &ph_table[i];

		if (ph->p_type == PT_LOAD) {
			if ((err = elf_read_segment(fd, ph, (void *) base_addr + ph->p_vaddr))) {
				free(ph_table);
				return err;
			}
		}
	}

	free(ph_table);
	close(fd);

	exec->base_addr = base_addr;
	exec->interp_entry = base_addr + header.e_entry;

	return ENOERR;
}

static int load_exec(char *filename, exec_t *exec) {
	Elf32_Ehdr header;
	size_t size;
	Elf32_Phdr *ph_table;
	Elf32_Phdr *ph;
	struct marea *marea;
	int err;

	int fd = open(filename, O_RDONLY);

	if (fd == -1) {
		return -EBADF;
	}

	if ((err = elf_read_header(fd, &header))) {
		return err;
	}

	if (header.e_type != ET_EXEC) {
		return -EBADF;
	}

	size = header.e_phnum * header.e_phentsize;
	if (!(ph_table = malloc(size))) {
		return -ENOMEM;
	}
	elf_read_ph_table(fd, &header, ph_table);

	for (int i = 0; i < header.e_phnum; i++) {
		ph = &ph_table[i];

		if (ph->p_type == PT_PHDR) {
			exec->phdr = ph->p_vaddr;
			continue;
		}

		if (ph->p_type == PT_INTERP) {

		}

		if (ph->p_type != PT_LOAD) {
			continue;
		}

		marea = mmap_place_marea(task_self()->mmap, ph->p_vaddr, ph->p_vaddr + ph->p_memsz, 0);

		if (!marea) {
			free(ph_table);
			return -ENOMEM;
		}

		if ((err = elf_read_segment(fd, ph, (void *) ph->p_vaddr))) {
			free(ph_table);
			return err;;
		}
	}

	free(ph_table);
	close(fd);

	load_interp("ld.so", exec);

	exec->filename = filename;
	exec->entry = header.e_entry;
	exec->phent = header.e_phentsize;
	exec->phnum = header.e_phnum;

	return ENOERR;
}

int elf_exec(char *filename) {
	struct ue_data ue_data;
	uint32_t entry;
	uint32_t stack;
	int err;
	exec_t exec;

	if ((err = load_exec(filename, &exec))) {
		return err;
	}

	stack = mmap_create_stack(task_self()->mmap);

	fill_stack(&stack, &exec);

	entry = exec.interp_entry ? exec.interp_entry : exec.entry;

	ue_data.ip = (void *) entry;
	ue_data.sp = (void *) stack;

	usermode_entry(&ue_data);

	return -EINTR;
}
