/*
 * This file is part of EMBOX.
 */

/**
 * @file
 *
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
	__ipstack_packets_start = .;   \
		*(.ipstack.packets)        \
	__ipstack_packets_end = .;     \
                                   \
	ALIGNMENT();                   \
	__ipstack_sockets_start = .;   \
		*(.ipstack.sockets)        \
	__ipstack_sockets_end = .;     \
                                   \
	ALIGNMENT();                   \
	__ipstack_protos_start = .;    \
		*(.ipstack.protos)         \
	__ipstack_protos_end = .;      \
                                   \
	ALIGNMENT();                   \
	*(.checksum)                   \

/* Allocates memory for express test's results.
 *
 * For each test we have it's int return code.
 *
 * It is the same size as the section
 * for express tests descriptors.
 * So here (.express_tests) is written.
 *
 * Also, we know exact size of the section,
 * so nothing like "__expr_tst_end"
 * is written in the end.
 *
 * This section is allocated in .bss
 * it won't be cleaned in asm code!
 */
#define ALLOC_EXPRESS_RESULT \
	ALIGNMENT(); \
	__express_tests_result = .; \
	*(.express_tests)



#endif /* EMBOX_LDS_H_ */
