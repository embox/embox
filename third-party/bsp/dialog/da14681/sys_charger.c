/*
 * sys_charger.c
 *
 *  Created on: May 8, 2020
 *      Author: anton
 */
#include <stdint.h>

void hw_charger_usb_cb(uint16_t status)
{
#if 0
        if (hw_charger_check_contact(status)) {
                usb_contact = true;
                hit_time = timer_cnt;
        } else {
                usb_contact = false;
                hit_time = 0xFFFFFF;
        }

        /* Disable interrupt to avoid toggling during debouncing. The interrupt will be re-enabled
         * by the FSM and the status will be updated at that point in time.
         */
        hw_charger_disable_charger_irq();
#endif
}

void hw_charger_suspended_cb(void)
{
#if 0
        pm_stay_idle();

        usb_is_suspended();
#endif
}

void hw_charger_resumed_cb(void)
{
#if 0
        pm_resume_sleep();
#endif
}
