/*
 * This file is part of EMBOX.
 */

/**
 * @file
 * @date 10.12.2009
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_LDS_H_
#define EMBOX_LDS_H_

#define __SECTION_SYMBOLS(var_prefix, section_name) \
	var_prefix ## _vma =     ADDR(section_name); \
	var_prefix ## _lma = LOADADDR(section_name); \
	var_prefix ## _len =   SIZEOF(section_name); \

#define SECTION_SYMBOLS(section) \
	__SECTION_SYMBOLS(_##section, .##section) \

#define MEMORY_REGION(name) \
	name : ORIGIN = LDS_REGION_BASE_##name, LENGTH = LDS_REGION_SIZE_##name

#define SECTION_REGION(section) \
	> LDS_SECTION_VMA_##section AT> LDS_SECTION_LMA_##section

#define ALIGNMENT() . = ALIGN(0x8)

#define LDS_INPUT_RODATA \
	ALIGNMENT();                   \
                                   \
	*(.rodata*)                    \
	*(.const)                      \
                                   \
	ALIGNMENT();                   \
	__express_tests_start = . ;    \
		*(.express_tests)          \
	__express_tests_end = .;       \
                                   \
	ALIGNMENT();                   \
	__modules_handlers_start = .;  \
		*(.modules_handlers)       \
	__modules_handlers_end = .;    \
                                   \
	ALIGNMENT();                   \
	__init_handlers_start = .;     \
		*(.init_handlers)          \
	__init_handlers_end = .;       \
                                   \
	ALIGNMENT();                   \
	__shell_commands_start = .;    \
		*(.shell_commands)         \
	__shell_commands_end = .;      \
                                   \
	ALIGNMENT();                   \
	*(.checksum)                   \

#endif /* EMBOX_LDS_H_ */
