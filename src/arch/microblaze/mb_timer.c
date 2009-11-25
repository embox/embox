/**
 * @file mb_timer.c
 * @brief Implementation for microblaze timer
 * @date 19.11.2009
 * @author Anton Bondarev
 */

#include "asm/cpu_conf.h"
#include "kernel/irq.h"

#define TIMER_ENABLE_ALL    0x400 /**< ENALL */
#define TIMER_PWM           0x200 /**< PWMA0 */
#define TIMER_INTERRUPT     0x100 /**< T0INT */
#define TIMER_ENABLE        0x080 /**< ENT0 */
#define TIMER_ENABLE_INTR   0x040 /**< ENIT0 */
#define TIMER_RESET         0x020 /**< LOAD0 */
#define TIMER_RELOAD        0x010 /**< ARHT0 */
#define TIMER_EXT_CAPTURE   0x008 /**< CAPT0 */
#define TIMER_EXT_COMPARE   0x004 /**< GENT0 */
#define TIMER_DOWN_COUNT    0x002 /**< UDT0 */
#define TIMER_CAPTURE_MODE  0x001 /**< MDT0 */

typedef volatile struct microblaze_timer_t {
	int ctrl;            /**< control/status register TCSR */
	int ld;              /**< load register TLR */
	int cnt;             /**< timer/counter register */
} microblaze_timer_t;


int timers_ctrl_init(IRQ_HANDLER irq_handler) {
	return 0;
}

