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
#include <getopt.h>
#include <types.h>
#include <hal/mm/mmu_core.h>
#include <stdio.h>

EMBOX_EXAMPLE(exec);

#define PAGE_SIZE 0x1000


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
typedef void (*vfunc_t)(void);
static void  __attribute__((aligned(PAGE_SIZE))) function1(void) {
	printf("\n\tInside the first function\n");
}

static void __attribute__((aligned(PAGE_SIZE))) function2(void) {
	printf("\n\tInside the second function\n");
}

//static uint32_t mmu_translate(uint32_t phy_addr) {
//	return 0;
//}

static int mmu_probe(void) {
	vfunc_t vfunc = 0;
	/* one-on-one mapping for context 0 */
//	mmu_map_region(0, 0, 0x1000000, /* MMU_PTE_PRIV */ 0x000000000, 0x0);
//	mmu_map_region(0x44000000, 0x44000000, 0x1000000, /* MMU_PTE_PRIV */ 0x00000000, 0x0);
//	mmu_map_region(0x80000000, 0x80000000, 0x1000000, /* MMU_PTE_PRIV */ 0x00000000, 0x0);


	/* close your eyes and pray ... */
	printf("\nPaging starting...\n");

	/* enabling paging */
	mmu_on();

	mmu_map_region(0, (paddr_t )function1, (vaddr_t)vfunc, PAGE_SIZE, 0);
	vfunc();

	mmu_map_region(0, (paddr_t )function2, (vaddr_t)vfunc, PAGE_SIZE, 0);
	vfunc();



	printf ("\nEnding mmu testing...\n");

	/* disabling paging */
	mmu_off();

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
