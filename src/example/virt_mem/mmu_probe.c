/**
 * @file
 * @brief testing mmu module
 *
 * @date 09.06.09
 * @author Anton Bondarev
 *
 * @author Gleb Efimov
 */

#include <embox/example.h>
#include <unistd.h>
#include <stdint.h>
#include <hal/mm/mmu_core.h>
#include <stdio.h>

EMBOX_EXAMPLE(exec);


#if 0
static void print_usage(void) {
	printf("Usage: mmu_probe [-r] [-h]\n");
}

static bool mmu_show_ctrl() {

	unsigned int ctrl_reg = mmu_get_mmureg(LEON_CNR_CTRL);
	printf("CTLR REG:\t0x%08X\n", ctrl_reg);
	printf("\tIMPL:\t0x%01X\n", (ctrl_reg & MMU_CTRL_IMPL) >> 28);
	printf("\tVER:\t0x%01X\n",  (ctrl_reg & MMU_CTRL_VER) >> 24);
	printf("\tSC:\t0x%04X\n",   (ctrl_reg & MMU_CTRL_SC) >> 8);
	printf("\tPSO:\t%d\n",      (ctrl_reg & MMU_CTRL_PSO) >> 7);
	printf("\tNF:\t%d\n",       (ctrl_reg & MMU_CTRL_NF) >> 1);
	printf("\tE:\t%d\n",         ctrl_reg & MMU_CTRL_E);

	return 0;
}

static bool mmu_show_fault_status() {
	unsigned int fault_reg = mmu_get_mmureg(LEON_CNR_F);

	printf("FAULT STATUS:\t0x%08X\n", fault_reg);
	printf("\tEBE:\t0x%02X\n", (fault_reg & MMU_F_EBE) >> 10);
	printf("\tL:\t0x%01X\n",   (fault_reg & MMU_F_L) >> 8);
	printf("\tAT:\t0x%01X\n",  (fault_reg & MMU_F_AT) >> 5);
	printf("\tFT:\t0x%01X\n",  (fault_reg & MMU_F_FT) >> 2);
	printf("\tFAV:\t%d\n",     (fault_reg & MMU_F_FAV) >> 1);
	printf("\tOW:\t%d\n",       fault_reg & MMU_F_OW);

	return 0;
}
#endif
#if 0
/**
 * show MMU register
 */
static bool mmu_show_reg(void) {

	printf("Registers MMU:\n");
	mmu_show_ctrl();
	printf("CTXTBL PTR:\t0x%08X\n", mmu_get_mmureg(LEON_CNR_CTXP));
	printf("CTX REG:\t0x%08X\n", mmu_get_mmureg(LEON_CNR_CTX));
	mmu_show_fault_status();
	printf("FAULT ADDR:\t0x%08X\n", mmu_get_mmureg(LEON_CNR_CTX));

	return 0;
}
#endif
typedef int (*vfunc_t)(void);
static int  __attribute__((aligned(MMU_PAGE_SIZE))) function1(void) {
	return 1;
}

static int __attribute__((aligned(MMU_PAGE_SIZE))) function2(void) {
	return 2;
}



extern void mmu_flush_tlb(void);

static int mmu_probe(void) {
	vfunc_t vfunc = (void*)0x0;
	int res;

	/* close your eyes and pray ... */
	printf("\nPaging starting...\n");

	/* enabling paging */
	mmu_on();
	/* map first function to address 0 */
	mmu_map_region(0, (paddr_t )function1, (vaddr_t)vfunc, MMU_PAGE_SIZE, MMU_PAGE_WRITEABLE);
	/* call function from address 0 */
	res = vfunc();
	printf("from address 0x%X called function %d \n", (uint32_t)vfunc, res);

	/* map second function to address 0 */
	mmu_map_region(0, (paddr_t )function2, (vaddr_t)vfunc, MMU_PAGE_SIZE, MMU_PAGE_WRITEABLE);
	/* call function from address 0 */
	res = vfunc();
	printf("from address 0x%X called function %d \n", (uint32_t)vfunc, res);


	/* disabling paging */
	mmu_off();
	printf ("\nEnding mmu testing...\n");
	return 0;
}

/**
 * handler of command "mmu_probe"
 * It starts tests of mmu mode
 * return 0 if success
 * return -1 another way
 */
static int exec(int argc, char **argv) {
#if 0
	int opt;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "rh"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		case 'r':
			mmu_show_reg();
			return 0;
		case 'p':
			mmu_translate(0);
			return 0;
		default:
			return 0;
		}
	}
#endif
	return mmu_probe();
}
