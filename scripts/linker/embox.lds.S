
OUTPUT_FORMAT("elf32-microblaze", "elf32-microblaze", "elf32-microblaze")
OUTPUT_ARCH(microblaze)
ENTRY(_hardreset)
/*OUTPUT_ARCH(sparc)
__DYNAMIC  =  0;
ENTRY(_hardreset)
*/

#include "linker_map.h"
/* these are the maximum values */

MEMORY
{
  DDR_REGION   : ORIGIN = DDR_START, LENGTH = DDR_LENGTH
  ALING_REGION : ORIGIN = INTERNAL_START, LENGTH = INTERNAL_LENGTH
}

/*
 * Initalize some symbols to be zero so we can reference them in the
 * crt0 without core dumping. These functions are all optional, but
 * we do this so we can have our crt0 always use them if they exist.
 * This is so BSPs work better when using the crt0 installed with gcc.
 * We have to initalize them twice, so we cover a.out (which prepends
 * an underscore) and coff object file formats.
 */
PROVIDE (hardware_init_hook = 0);
PROVIDE (_hardware_init_hook = 0);
PROVIDE (software_init_hook = 0);
PROVIDE (_software_init_hook = 0);

SECTIONS {
	.text :
	{
		. = ALIGN (0x1000);
		_trap_table = .;
		*(.trap_table)

		. = ALIGN (0x800);
		*(.recovery_section)

		. = ALIGN (0x1000);
		_text_start = .;
		_text_start = .;
		*(.text)
		. = ALIGN (0x4);
		*(.rodata*)
		. = ALIGN(0x4);
		*(.const)
		. = ALIGN(0x4);

		_sinittext = .;
		    *(.init.text)
        _einittext = .;

		__express_tests_start = . ;
	    *(.express_tests)
	    __express_tests_end = .;

	    . = ALIGN(0x4);
	    __modules_handlers_start = .;
	    *(.modules_handlers)
	    __modules_handlers_end = .;

	    . = ALIGN(0x4);
	    __init_handlers_start = .;
	    *(.init_handlers)
	    __init_handlers_end = .;

	    . = ALIGN(0x4);
	    __shell_commands_start = .;
	    *(.shell_commands)
	    __shell_commands_end = .;

	    . = ALIGN(0x4);
	    _piggy_start = .;
	    _piggy_end = .;
	    . = ALIGN(0x4);

        *(.checksum)
        . = ALIGN(0x4);
		_endtext = .;

		_data_image_start = .;
	}> DDR_REGION

	.data :
	{
	    . = ALIGN(0x4);
	    _data_start = .;
	    *(.data)
		_data_end = .;
	} > DDR_REGION

	.bss :
	{
		. = ALIGN(0x4);
	    _bstart = . ; *(.bss) *(COMMON) ;
	    _bend = ALIGN(0x8);
	} > DDR_REGION
    _stack = _bend + STACK_SIZE;
	PROVIDE (__stack = _bend + 0x100000);
	.internal_mem :
	{
		. = ALIGN(0x8);
		_internal_start = .;
		*(.internal_mem)
		_internal_end = .;
	}>ALING_REGION
	PROVIDE (__internal_stack = INTERNAL_START + 0x800);
}
