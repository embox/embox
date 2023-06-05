/**
 * @file
 * @brief Demonstrates data & bss sections addresses
 *
 * @date 05.06.23
 * @author Anton Bondarev
 */

#include <stdio.h>

#include <framework/mod/mod_sect_info.h>

static void show_mod_symbols(void) {
	struct mod_sect_info sect_info;

	MOD_SELF_GET_SECT_INFO(sect_info);

	printf("mod sect info:\n");
	printf("\tdata_vma = (0x%p)\n", sect_info.data_vma);
	printf("\tdata_vma_end = (0x%p)\n", sect_info.data_vma_end);
	printf("\tbss_vma = (0x%p)\n", sect_info.bss_vma);
	printf("\tbss_vma_end = (0x%p)\n", sect_info.bss_vma_end);
}
extern int _bss_vma, _bss_len;
extern int _data_vma, _data_lma, _data_len;

static void show_global_symbols(void) {
	printf("system sect info:\n");

	printf("data: _data_vma=%p:%p,_data_lma=%p, _data_len=0x%x\n", 
		&_data_vma, 
		((void *)&_data_vma) + (unsigned int)&_data_len,
		 &_data_lma,
		 (unsigned int)&_data_len);

	printf("bss: _bss_vma=%p:%p, _bss_len=0x%x\n", 
		&_bss_vma,
		((void *)&_bss_vma) + (unsigned int)&_bss_len,
		 (unsigned int)&_bss_len);
}

int main(int argc, char **argv) {
	show_global_symbols();

	show_mod_symbols();

    return 0;
}
