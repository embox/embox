/* It's required to provide some undefined apps symbols.
 * usermode86 doesn't use mk/image.lds.S like other arch's.
 * This code should be checked if we want to use apps for usermode86.
 */

#include <asm-generic/embox.lds.h>
#include <apps.lds.h>

SECTIONS {
	.text : {
		LDS_MODULES_TEXT
	}

	.rodata : {
		LDS_MODULES_RODATA
	}

	.data : {
		LDS_MODULES_DATA
		_app_data_vma = .;
		LDS_APPS_DATA
	}

	.bss : {
		LDS_MODULES_BSS
		LDS_APPS_BSS
		LDS_APP_DATA_RESERVE_BSS
	}
}
