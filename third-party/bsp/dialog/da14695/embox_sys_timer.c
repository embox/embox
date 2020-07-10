/*
 * embox_sys_timer.c
 *
 *  Created on: Jul 10, 2020
 *      Author: anton
 */

#include <stdint.h>

#include <config/custom_config_qspi.h>
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

extern __RETAINED_CODE void update_timestamp_values(void);

extern __RETAINED uint32_t current_time;
extern __RETAINED uint64_t sys_rtc_time;

extern __RETAINED uint64_t sys_rtc_clock_time_fine;         // counts time in usec*1024*1024
extern __RETAINED uint64_t sys_rtc_clock_time;         // counts time in usec

extern __RETAINED_CODE uint32_t sys_timer_advance_time_compute(uint32_t *trigger);
extern __RETAINED_CODE void sys_timer_advance_time_apply(uint32_t trigger);

__RETAINED_CODE uint32_t sys_timer_update_slept_time(void)
{
        uint32_t trigger;
        uint32_t elapsed_ticks;
        /*
         * Update Real Time Clock value and calculate the time spent sleeping.
         * lp_prescaled_time - lp_last_trigger : sleep time in lp cycles
         * lp_previous_time : lp timer offset at sleep entry
         */
#if (SYS_TIM_DEBUG == 1)
        trigger_hit_at_ret = hw_timer_get_count(HW_TIMER2);
#endif

        // Calculate time spent sleeping in ticks and the offset in this tick period.
        elapsed_ticks = sys_timer_advance_time_compute(&trigger);

        // Advance time
        if (elapsed_ticks > 0) {
   //             xTaskIncrementTick();
   //             vTaskStepTick( elapsed_ticks - 1 );
        }

        sys_timer_advance_time_apply(trigger);

        return elapsed_ticks;
}

uint64_t sys_timer_get_uptime_ticks(void)
{
 //       vPortEnterCritical();
        update_timestamp_values();
 //       vPortExitCritical();

        return sys_rtc_time;
}

__RETAINED_CODE uint64_t sys_timer_get_uptime_ticks_fromISR(void)
{
 //       uint32_t ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();
        update_timestamp_values();
  //      portCLEAR_INTERRUPT_MASK_FROM_ISR( ulPreviousMask );

        return sys_rtc_time;
}

uint64_t sys_timer_get_uptime_usec(void)
{
 //       vPortEnterCritical();
        update_timestamp_values();
  //      vPortExitCritical();

#if (dg_configUSE_LP_CLK == LP_CLK_RCX)
        return sys_rtc_clock_time + (sys_rtc_clock_time_fine >> 20);
#else
        return (sys_rtc_time * 1000000) / configSYSTICK_CLOCK_HZ;
#endif
}

__RETAINED_CODE uint64_t sys_timer_get_uptime_usec_fromISR(void)
{
  //      uint32_t ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();
        update_timestamp_values();
  //      portCLEAR_INTERRUPT_MASK_FROM_ISR( ulPreviousMask );

#if (dg_configUSE_LP_CLK == LP_CLK_RCX)
        return sys_rtc_clock_time + (sys_rtc_clock_time_fine >> 20);
#else
        return (sys_rtc_time * 1000000) / configSYSTICK_CLOCK_HZ;
#endif
}


__RETAINED_CODE uint64_t sys_timer_get_timestamp_fromCPM(uint32_t* timer_value)
{
        update_timestamp_values();
        *timer_value = current_time;

        return sys_rtc_time;
}

uint64_t* sys_timer_get_rtc_time(void)
{
        return &sys_rtc_time;
}

uint32_t* sys_timer_get_current_time(void)
{
        return &current_time;
}
