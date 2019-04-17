/**
 * @file
 *
 * @date Apr 13, 2019
 * @author: Anton Bondarev
 */


#include <stdio.h>
#include <unistd.h>

#include <util/math.h>

#include <hal/mmu.h>

#define MMUINFO_BUFF_SZ 0x10

static void print_usage(void) {
	printf("Usage: mmuinfo [-h]\n");
}

int main(int argc, char **argv) {
	int opt;
	int i;
	int offset;
	int mmu_regs_table_size;
	struct mmuinfo_regs mmuinfo_buf[MMUINFO_BUFF_SZ];

	while (-1 != (opt = getopt(argc, argv, "hc"))) {
		switch (opt) {
		case 'h':
		default:
			print_usage();
			return 0;
		}
	}

	mmu_regs_table_size = arch_mmu_get_regs_table_size();
	printf("MMU controller registers:\n");
	for (offset = 0; offset < mmu_regs_table_size; offset += MMUINFO_BUFF_SZ) {
		int cnt;

		cnt = arch_mmu_get_regs_table(mmuinfo_buf, MMUINFO_BUFF_SZ, offset);
		cnt = min(MMUINFO_BUFF_SZ, cnt);
		for(i = 0; i < cnt; i ++) {
			printf("%d: %s (0x%" PRIxMMUREG ")\n", i, mmuinfo_buf[i].mmu_reg_name, mmuinfo_buf[i].mmu_reg_value);
		}
	}
	return 0;
}
