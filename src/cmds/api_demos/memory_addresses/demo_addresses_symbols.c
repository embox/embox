/**
 * @file
 * @brief Demonstrates data & bss sections addresses
 *
 * @date 05.06.23
 * @author Anton Bondarev
 */

#include <stdio.h>

#include <framework/mod/mod_sect_info.h>

int main(int argc, char **argv) {
	struct mod_sect_info sect_info;

	MOD_SELF_GET_SECT_INFO(sect_info);

	printf("mod sect info:\n");
	printf("\tdata_vma = (0x%p)\n", sect_info.data_vma);
	printf("\tdata_vma_end = (0x%p)\n", sect_info.data_vma_end);
	printf("\tbss_vma = (0x%p)\n", sect_info.bss_vma);
	printf("\tbss_vma_end = (0x%p)\n", sect_info.bss_vma_end);

    return 0;
}