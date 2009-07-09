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
#include "module.h"


void copy_data_section()
{
    extern char _endtext, _data_start, _data_end, _bstart, _bend;

    char *src = &_endtext;

    char *dst = &_data_start;


    if (0x40000000 >= (unsigned int)&_endtext ){
    /* ROM has data at end of text; copy it. */
    while (dst < &_data_end) {
        *dst++ = *src++;
    }
    }

    /* Zero bss */
    for (dst = &_bstart; dst< &_bend; dst++)
        *dst = 0;
}
static int init_modules(){
    extern char __modules_handlers_start, __modules_handlers_end;
    MODULE_HANDLER ** handler = (MODULE_HANDLER **)&__modules_handlers_start;
    for(; (unsigned int)handler < (unsigned int)&__modules_handlers_end; handler ++){
        if (NULL == (*handler)){
            TRACE("Error: Wrong module handler\n");
            return -1;
        }
        if (NULL == ((*handler)->name)){
            TRACE("Error: Wrong express test handler. Can't find get_test_name function\n");
            return -1;
        }
        if (NULL == ((*handler)->init)){
            TRACE("Error: Wrong express test handler. Can't find exec function for test %s\n", (*handler)->name);
            return -1;
        }

        if (-1 == (*handler)->init()){
            continue;
        }
        TRACE("module %s ... was loaded\n", (*handler)->name);

        return 0;
    }
}
int hardware_init_hook() {
    //TODO during too long time for simulation:(
    copy_data_section();

    cache_data_enable();
    cache_instr_enable();

    irq_init_handlers();
    uart_init();
    timers_init();

    init_modules();


    TRACE("start...\n");
#ifndef SIMULATION_TRG
    eth_init();//interfaces
    icmp_init();
    udp_init();

#endif /* SIMULATION_TRG */

#ifdef MONITOR_FS
    rootfs_init();
#endif //MONITOR_FS

    return 0;
}
