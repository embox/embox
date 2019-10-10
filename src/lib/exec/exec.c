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
#include <unistd.h>
#include <util/math.h>
#include <util/binalign.h>
#include <sys/mman.h>

#include <lib/libelf.h>
#include <kernel/usermode.h>
#include <mem/mmap.h>
#include <mem/vmem.h>
#include <mem/phymem.h>
#include <kernel/task.h>
#include <kernel/task/resource/mmap.h>
#include <kernel/task/resource/task_phymem.h>

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
	const char *filename;
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

static inline void stack_push_aux(uint32_t *stack, uint32_t type, uint32_t val) {
	stack_push_int(stack, val);
	stack_push_int(stack, type);
}

static void fill_stack(uint32_t *stack, exec_t *exec, char *const argv[], char *const envp[]) {
	int argc = 0, envc = 0;
	uint32_t sp;

	/* Counting parameters */
	while (argv[argc]) argc++;
	while (envp[envc]) envc++;

	// End marker. Is it necessary?
	sp = stack_push_int(stack, 0);

	/* Pushing envp strings */
	for (int i = envc - 1; i >= 0; i--) {
		stack_push_str(stack, envp[i]);
	}

	/* Pushing argv strings */
	for (int i = argc - 1; i >= 0; i--) {
		stack_push_str(stack, argv[i]);
	}

	// Padding. Is it necessary?
	stack_push_int(stack, 0);

	/* Pushing auxiliary data */
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

	/* Pushing envp */
	stack_push_int(stack, 0);
	for (int i = envc - 1; i >= 0; i--) {
		sp -= strlen(envp[i]) + 1;
		stack_push_int(stack, sp);
	}

	/* Pushing argv */
	stack_push_int(stack, 0);
	for (int i = argc - 1; i >= 0; i--) {
		sp -= strlen(argv[i]) + 1;
		stack_push_int(stack, sp);
	}

	/* Pushing argc */
	stack_push_int(stack, argc);
}

static int load_interp(char *filename, exec_t *exec) {
	Elf32_Ehdr header;
	Elf32_Phdr *ph_table, *ph;
	Elf32_Addr base_addr;
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

	base_addr = mmap_alloc(task_self_resource_mmap(), size);
	if (base_addr == 0) {
		free(ph_table);
		return -ENOMEM;
	}

	mmap_place(task_self_resource_mmap(), base_addr, size, 0);
	vmem_map_region(vmem_current_context(), base_addr, base_addr, size,
			PROT_WRITE | PROT_READ | PROT_EXEC | VMEM_PAGE_USERMODE);

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

static int load_exec(const char *filename, exec_t *exec) {
	Elf32_Ehdr header;
	size_t size;
	Elf32_Phdr *ph_table;
	Elf32_Phdr *ph;
	int err;
	void *paddr;
	char interp[255];
	int has_interp = 0;

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
			if ((err = elf_read_interp(fd, ph, interp))) {
				free(ph_table);
				return err;
			}
			has_interp = 1;

			continue;
		}

		if (ph->p_type != PT_LOAD) {
			continue;
		}
#if 0
		pa = mmap((void *)ph->p_vaddr, ph->p_memsz, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_FIXED, 0, 0);
		if (MAP_FAILED == pa) {
			free(ph_table);
			return -1;
		}
#else
		size = binalign_bound(ph->p_memsz, MMU_PAGE_SIZE);
		if (mmap_place(task_self_resource_mmap(), ph->p_vaddr, size,
					PROT_EXEC | PROT_READ | PROT_WRITE)) {
			free(ph_table);
			return -ENOMEM;
		}

		paddr = phymem_alloc(size);
		task_resource_phymem_add(task_self(), paddr, size / MMU_PAGE_SIZE);

		vmem_map_region(vmem_current_context(),
				(mmu_paddr_t) paddr,
				ph->p_vaddr,
				size,
				PROT_WRITE | PROT_READ | PROT_EXEC | VMEM_PAGE_USERMODE);

		/* XXX brk is a max of ph's right sides. It unaligned now! */
		mmap_set_brk(task_self_resource_mmap(),
			max(mmap_get_brk(task_self_resource_mmap()), (void *) ph->p_vaddr + ph->p_memsz));

#endif
		if ((err = elf_read_segment(fd, ph, (void *) ph->p_vaddr))) {
			free(ph_table);
			return err;
		}
	}

	free(ph_table);
	close(fd);

	if (has_interp) {
		if ((err = load_interp(interp, exec))) {
			return err;
		}
	}

	exec->filename = filename;
	exec->entry = header.e_entry;
	exec->phent = header.e_phentsize;
	exec->phnum = header.e_phnum;

	return ENOERR;
}

uint32_t mmap_create_stack(struct emmap *mmap) {
	size_t size = 4096;
	uintptr_t base = mmap_alloc(mmap, size);
	mmap_place(task_self_resource_mmap(), base, size, 0);
	vmem_map_region(mmap->ctx, base, base, size,
			PROT_WRITE | PROT_READ | PROT_EXEC | VMEM_PAGE_USERMODE);
	return base + size;
}

//extern uint32_t mmap_userspace_create(struct emmap *emmap, size_t stack_size);
int execve_syscall(const char *filename, char *const argv[], char *const envp[]) {
	struct ue_data ue_data;
	uint32_t entry;
	uint32_t stack;
	int err;
	exec_t exec;
	struct emmap *emmap;

	emmap = task_self_resource_mmap();

	sched_lock();
	mmu_set_context(emmap->ctx);

	memset(&exec, 0, sizeof(exec_t));

	mmap_set_brk(emmap, NULL);
	if ((err = load_exec(filename, &exec))) {
		SET_ERRNO(-err);
		return -1;
	}

	stack = mmap_create_stack(emmap);

	fill_stack(&stack, &exec, argv, envp);

	entry = exec.interp_entry ? exec.interp_entry : exec.entry;

	ue_data.ip = (void *) entry;
	ue_data.sp = (void *) stack;

	usermode_entry(&ue_data);

	mmu_set_context(1);
	sched_unlock();

	SET_ERRNO(EINTR);
	return -1;
}
