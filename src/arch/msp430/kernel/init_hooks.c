/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.08.2013
 */

#include <string.h>

#include <hal/reg.h>

#define TAG_DCO_30 ((char *) 0x10f6)

#define CALDCO_16 (TAG_DCO_30 + 2)
#define CALBC1_16 (TAG_DCO_30 + 3)

#define DCOCTL  0x56
#define BCSCTL1	0x57

extern int _bss_vma, _bss_len;
extern int _data_vma, _data_lma, _data_len;

void hardware_init_hook(void) {
	/*initialize main clock to 16Mhz */
	REG_STORE(DCOCTL, 0);
	REG_STORE(BCSCTL1, *CALBC1_16);
	REG_STORE(DCOCTL, *CALDCO_16);
}

void software_init_hook(void) {

	memcpy(&_data_vma, &_data_lma, (size_t) &_data_len);
	memset(&_bss_vma, 0, (size_t) &_bss_len);
}
