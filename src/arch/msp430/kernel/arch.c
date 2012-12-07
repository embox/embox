/**
 * @file
 * @brief
 *
 * @date 21.06.10
 * @author Anton Kozlov
 */

#include <string.h>

void arch_init(void) {

}

void arch_idle(void) {

}

void arch_shutdown(void) {
	while (1);
}

extern int _bss_vma, _bss_len;
extern int _data_vma, _data_lma, _data_len;

void software_init_hook(void) {
	memcpy(&_data_vma, &_data_lma, (size_t) &_data_len);

	memset(&_bss_vma, 0, (size_t) &_bss_len);

}


