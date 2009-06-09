#ifndef _TIMERS_H_
#define _TIMERS_H_

#ifndef __ASSEMBLER__

#include "types.h"

#ifdef LEON3
typedef struct _TIMERS_STRUCT {
	volatile unsigned int scaler_cnt;  /**< 0x00 */
	volatile unsigned int scaler_ld;   /**< 0x04 */
	volatile unsigned int config_reg;  /**< 0x08 */
	volatile unsigned int dummy1;      /**< 0x0C */
	volatile unsigned int timer_cnt1;  /**< 0x10 */
	volatile unsigned int timer_ld1;   /**< 0x14 */
	volatile unsigned int timer_ctrl1; /**< 0x18 */
	volatile unsigned int dummy2;      /**< 0x1C */
	volatile unsigned int timer_cnt2;  /**< 0x20 */
	volatile unsigned int timer_ld2;   /**< 0x24 */
	volatile unsigned int timer_ctrl2; /**< 0x28 */
} TIMERS_STRUCT;
#endif

#ifdef LEON2
typedef struct _TIMERS_STRUCT
{
	volatile unsigned int timer_cnt1; /**< 0x40 */
	volatile unsigned int timer_ld1;
	volatile unsigned int timer_ctrl1;
	volatile unsigned int wdog;
	volatile unsigned int timer_cnt2; /**<0x50 */
	volatile unsigned int timer_ld2;
	volatile unsigned int timer_ctrl2;
	volatile unsigned int dummy8;
	volatile unsigned int scaler_cnt; //0x60
	volatile unsigned int scaler_ld;
	volatile unsigned int dummy9;
	volatile unsigned int dummy10;
}TIMERS_STRUCT;
#endif

int timers_init ();
typedef void (*TIMER_FUNC)(UINT32 id);

BOOL set_timer(UINT32 id, UINT32 ticks, TIMER_FUNC handle);
void close_timer (UINT32 id);

void sleep_1ms_irq (int ms);
inline void sleep (int ms);
void calibrate_sleep ();
UINT32 get_sys_time ();

int timers_context_save ();
int timers_context_restore (int context_number);

//typedef struct _TIMERS_ALIGN
//{
//	unsigned int scaler_cnt;
//	unsigned int scaler_ld;
//	unsigned int config_reg;
//	unsigned int timer_cnt1;
//	unsigned int timer_ld1;
//	unsigned int timer_ctrl1;
//	unsigned int timer_cnt2;
//	unsigned int timer_ld2;
//	unsigned int timer_ctrl2;
//}TIMERS_ALIGN;

//static TIMERS_ALIGN align;

#endif //__ASSEMBLER

//#define TIMERS_BASE 0x80000300
//#define TIMERS_ALIGN_ADDRESS &align

#endif //_TIMERS_H_
