/**
 * @file
 *
 * @brief AT91 Timer Counter interface
 *
 * @date 16.11.2011
 * @author Anton Kozlov
 */

#ifndef TIMER_COUNTER_H_
#define TIMER_COUNTER_H_

#include <drivers/at91sam7_tcc.h>

/**
 * Modes for input configuration
 * For each DIVx mode clock is Main Clock / (2 ^ x)
 */
#define	TC_INPUT_MODE_DIV1 AT91C_TC_CLKS_TIMER_DIV1_CLOCK
#define TC_INPUT_MODE_DIV2 AT91C_TC_CLKS_TIMER_DIV2_CLOCK
#define TC_INPUT_MODE_DIV3 AT91C_TC_CLKS_TIMER_DIV3_CLOCK
#define TC_INPUT_MODE_DIV4 AT91C_TC_CLKS_TIMER_DIV4_CLOCK
#define TC_INPUT_MODE_DIV5 AT91C_TC_CLKS_TIMER_DIV5_CLOCK


/**
 * Timer init
 */
extern void tc_init(uint8_t channel);

/**
 * Configure timer for counting external events
 */
extern void tc_config_input(uint8_t channel, uint32_t clock_mode);

/**
 * Run & zero counter
 */
extern void tc_reset(uint8_t channel);

/**
 * Get counted events
 * @return Counted events
 */
uint32_t tc_counter_value(uint8_t channel);

/**
 * Free hardware
 */
extern void tc_stop(uint8_t channel);

/**
 * Set limit for counter
 */
void tc_set_limit(uint8_t channel, uint32_t limit);

/**
 * Enable interrupt on counter limit reaching
 */
void tc_limit_int_enable(uint8_t channel, irq_handler_t handler);

/**
 * Disable interrupt on counter limit reaching
 */
void tc_limit_int_disable(uint8_t channel);
#endif /*TIMER_COUNTER_H_*/

