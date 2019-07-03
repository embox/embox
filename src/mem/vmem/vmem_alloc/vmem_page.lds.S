#include <asm-generic/embox.lds.h>

#include <config/embox/arch/mmu.h>
#include <framework/mod/options.h>

SECTIONS {
	.bss.embox.vmem_pages (NOLOAD): ALIGN(OPTION_MODULE_GET(embox__arch__mmu, NUMBER, page_size)) {
		__vmem_pages_start = .;
		*(.bss.embox.vmem_pages)
		__vmem_pages_end = .;
	}
}
