/**
 * \file init.c
 *
 * \date May 21, 2009
 * \author anton
 * \author afomin
 */

#include "types.h"
#include "common.h"
#include "conio.h"
#include "kernel/irq_ctrl.h"
#include "kernel/uart.h"
#include "kernel/timers.h"
#include "net/net.h"
#include "fs/rootfs.h"
#include "asm/cache.h"
#include "kernel/init.h"

//TODO this code must move to boot.S file
void copy_data_section() {
	extern char _endtext, _data_start, _data_end, _bstart, _bend;

	char *src = &_endtext;

	char *dst = &_data_start;

	if (0x40000000 >= (unsigned int) &_endtext) {
		/* ROM has data at end of text; copy it. */
		while (dst < &_data_end) {
			*dst++ = *src++;
		}
	}

	/* Zero bss */
	for (dst = &_bstart; dst < &_bend; dst++)
		*dst = 0;
}

//static int init_modules() {
//	extern module_descriptor_t *__modules_handlers_start,
//			*__modules_handlers_end;
//	module_descriptor_t ** p_module = &__modules_handlers_start;
//	int i, total = (int) (&__modules_handlers_end - &__modules_handlers_start);
//
//	TRACE("\nInitializing modules (total: %d)\n\n", total);
//
//	for (i = 0; i < total; i++, p_module++) {
//		if (NULL == (*p_module)) {
//			LOG_ERROR("Missing module descriptor\n");
//			continue;
//		}
//		if (NULL == ((*p_module)->name)) {
//			LOG_ERROR("Broken module descriptor. Can't find module name\n");
//			continue;
//		}
//		if (NULL == ((*p_module)->init)) {
//			LOG_ERROR("Broken module descriptor. Can't find init function for module %s\n", (*p_module)->name);
//			continue;
//		}
//
//		TRACE("Initializing %s ... ", (*p_module)->name);
//		if (-1 != (*p_module)->init()) {
//			TRACE("DONE\n");
//		} else {
//			TRACE("FAILED\n");
//		}
//
//	}
//
//	TRACE("\n");
//
//	return 0;
//}
//static int init_others() {
//	extern init_descriptor_t *__init_handlers_start, *__init_handlers_end;
//	init_descriptor_t ** p_init_desc = &__init_handlers_start;
//	int i, total = (int) (&__init_handlers_end - &__init_handlers_start);
//
//	TRACE("\nInitializing others (total: %d)\n\n", total);
//
//	for (i = 0; i < total; i++, p_init_desc++) {
//		if (NULL == (*p_init_desc)) {
//			LOG_ERROR("Missing init descriptor\n");
//			continue;
//		}
//		if (NULL == ((*p_init_desc)->init)) {
//			LOG_ERROR("Broken init handler descriptor.");
//			continue;
//		}
//
//		if (-1 != (*p_init_desc)->init()) {
//			TRACE("DONE\n");
//		} else {
//			TRACE("FAILED\n");
//		}
//
//	}
//
//	TRACE("\n");
//
//	return 0;
//}

// This section is to guarantee that 0-level init is ok
// Order is significant!
DECLARE_INIT("cache", cache_init_func, INIT_HARDWARE_LEVEL);
DECLARE_INIT("irq", irq_init_func, INIT_HARDWARE_LEVEL);
DECLARE_INIT("uart", uart_init_func, INIT_HARDWARE_LEVEL);
DECLARE_INIT("timers", timers_init_func, INIT_HARDWARE_LEVEL);

static int cache_init_func() {
	cache_data_enable();
	cache_instr_enable();
	return 0;
}
static int irq_init_func() {
	irq_init_handlers();
	return 0;
}
static int timers_init_func() {
	return timers_init();
}
static int uart_init_func() {
	return uart_init();
}

#define ON_INIT_TRACE(...) \
	do { \
		if (level != 0) TRACE(__VA_ARGS__);\
	} while (0)

int hardware_init_hook() {
	extern init_descriptor_t *__init_handlers_start, *__init_handlers_end;
	init_descriptor_t ** p_init_desc = &__init_handlers_start;
	int i, total = (int) (&__init_handlers_end - &__init_handlers_start);
	int level = 0;
	const char *init_name;
	const char *default_init_name = "???";

	//TODO during too long time for simulation:(
	copy_data_section();

	for (level = 0; level <= INIT_MAX_LEVEL; level++) {
		ON_INIT_TRACE("\n********* Init level %d:\n", level);
		for (p_init_desc = &__init_handlers_start; p_init_desc
				< &__init_handlers_end; p_init_desc++) {
			init_name = default_init_name;
			if (NULL == (*p_init_desc)) {
				ON_INIT_TRACE("Missing init descriptor\n");
				continue;
			}
			if ((*p_init_desc)->level != level) {
				continue;
			}
			if (NULL != (*p_init_desc)->name) {
				init_name = ((*p_init_desc)->name);
			}
			if (NULL == ((*p_init_desc)->init)) {
				ON_INIT_TRACE("%s has a broken init handler descriptor.\n", init_name);
				continue;
			}
			ON_INIT_TRACE("*** Initializing %s\n", (*p_init_desc)->name);
			(*p_init_desc)->init();
		}
	}

	TRACE("\nStarting Monitor...\n");
	return 0;
}
