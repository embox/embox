/*
 * arch_main.c
 *
 *  Created on: May 11, 2020
 *      Author: anton
 */
#include <stdint.h>
#include <stdbool.h>
#define CFG_CON 1
#define BLE_CONNECTION_MAX_USER CFG_CON

#include "ke_task.h"


/*
 * This is the placeholder for CMAC code, data and mailbox sections.
 */
extern uint32_t __cmi_section_end__;

/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
/** Custom message handlers */
struct custom_msg_handler custom_msg_handlers[] =
{
        { 0, 0, NULL}   //it must end with func == NULL so that loop can terminate correctly
};


void crypto_init(void) {

}

void cmac_mem_ctrl_setup(uint32_t *cmac_code_base_addr, uint32_t *cmac_end_base_addr)
{
//    extern uint32_t cmi_fw_dst_addr;

//    *cmac_code_base_addr = (uint32_t) &cmi_fw_dst_addr;
//    *cmac_end_base_addr = (uint32_t) &__cmi_section_end__;
}

/*
 * @brief Retrieve current time.
 *
 * @return  current time in 10ms step.
 */
uint32_t timer_get_time(void)
{
	return 0;
}

/*
 * @brief Sets the absolute expiration time for the first timer in queue.
 *
 * @param to The absolute expiration time in units of 10ms
 */
void timer_set_timeout(uint32_t to)
{

}

/*
 * @brief Enables or disables the timer
 */
void timer_enable(bool enable)
{
}

/**
 ****************************************************************************************
 * @brief Configures CMAC parameters.
 *
 * @Note This function gets called during execution of lib_ble_stack_init() and
 *       lib_ble_stack_reset() functions, at a point where CMAC is ready to start executing
 *       its main() function. CMAC execution will resume when this function returns.
 ****************************************************************************************
 */
void cmac_config_table_setup(void)
{
}

