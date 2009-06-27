/**
 * \file init.c
 *
 * \date May 21, 2009
 * \author anton
 */
#include "types.h"
#include "common.h"
#include "cache.h"
#include "leon.h"
#include "memory_map.h"

//LEON_REGS * const l_regs = (PVOID) (RG_BASE);


void copy_data_section()
{
    extern char _data_image_start, _data_start, _data_end, _bstart, _bend;

    char *src = &_data_image_start;

    char *dst = &_data_start;


    if (0x40000000 >= (unsigned int)&_data_image_start ){
    /* ROM has data at end of text; copy it. */
    while (dst < &_data_end) {
        *dst++ = *src++;
    }
    }

    /* Zero bss */
    for (dst = &_bstart; dst< &_bend; dst++)
        *dst = 0;
}
/*
typedef void (*INIT_MODULE_CALLBACK)();

typedef struct _INIT_MODULE_INFO {
        INIT_MODULE_CALLBACK cb;
	int busy;
} INIT_MODULE_INFO;

INIT_MODULE_INFO modules[16];

int init_module(INIT_MODULE_CALLBACK cb) {
	int i;
	for (i = 0; i < array_len(modules); i++) {
		if(0 == modules[i].busy) {
	        	modules[i].cb = cb;
			modules[i].busy = 1;
		}
	}
}
*/
int init() {
    //TODO during too long time for simulation:(
    copy_data_section();

    cache_data_enable();
    cache_instr_enable();

/*    INIT_MODULE_CALLBACK cb_init;
    int i;
    for (i = 0; i < array_len(modules); i++) {
	    if(1 == modules[i].busy) {
                    cb_init = modules[i].cb;
		    cb_init();
            }
    }
*/
#ifdef MONITOR_DRIVERS_GAISLER_IRQ_CTRL
    irq_init_handlers();
#endif
#ifdef MONITOR_DRIVERS_GAISLER_UART
    uart_init();
#endif
#ifdef MONITOR_DRIVERS_GAISLER_TIMER
    timers_init();
#endif
    TRACE("start...\n");
#ifndef SIMULATION_TRG
    eth_init();//interfaces
    icmp_init();
    udp_init();
#endif /* SIMULATION_TRG */
    return 0;
}
