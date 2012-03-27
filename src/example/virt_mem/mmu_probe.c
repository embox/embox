/**
 * @file
 * @brief testing mmu module
 *
 * @date 09.06.09
 * @author Anton Bondarev
 *
 * @author Gleb Efimov
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <hal/mm/mmu_core.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: mmu_probe [-r] [-h]\n");
}

#ifdef 0
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

/**
 * show MMU register
 */
static bool mmu_show_reg() {
	printf("Registers MMU:\n");
	mmu_show_ctrl();
	printf("CTXTBL PTR:\t0x%08X\n", mmu_get_mmureg(LEON_CNR_CTXP));
	printf("CTX REG:\t0x%08X\n", mmu_get_mmureg(LEON_CNR_CTX));
	mmu_show_fault_status();
	printf("FAULT ADDR:\t0x%08X\n", mmu_get_mmureg(LEON_CNR_CTX));
	return 0;
}
#endif
#define TLBNUM 4

static bool mmu_probe() {
	/* alloc mem for pages */
	__asm__(
		".section .data\n\t"
		".align %0\n\t"
		"page0: .skip %0\n\t"
		"page1: .skip %0\n\t"
		"page2: .skip %4\n\t"
		".text\n"
		: : "i" (MMU_PAGE_SIZE),/* Page Size */
		"i"(MMU_GTABLE_SIZE) ,	/* Global page tables*/
		"i"(MMU_MTABLE_SIZE) ,	/* Middle page tables */
		"i"(MMU_PTABLE_SIZE) , 	/* Page table*/
		"i"((3)*MMU_PAGE_SIZE)
	);


	/* one-on-one mapping for context 0 */
	mmu_map_region(0, 0, 0x1000000, /* MMU_PTE_PRIV */ 0x00000000);
	mmu_map_region(0x44000000, 0x44000000, 0x1000000, /* MMU_PTE_PRIV */ 0x00000000);
	mmu_map_region(0x80000000, 0x80000000, 0x1000000, /* MMU_PTE_PRIV */ 0x00000000);

	/* close your eyes and pray ... */
	printf("mmu start...\n");
	mmu_on();

	printf ("ending mmu testing");
	mmu_off();
	return 0;
}

/**
 * handler of command "mmu_probe"
 * It starts tests of mmu mode
 * return 0 if successed
 * return -1 another way
 */
static int exec(int argc, char **argv) {
#ifdef 0
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
		default:
			return 0;
		}
	}
#endif
	return mmu_probe();
}
