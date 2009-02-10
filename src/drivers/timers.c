#include "types.h"
#include "leon_config.h"
#include "irq.h"
#include "plug_and_play.h"
#include "pnp_id.h"
#include "timers.h"

#ifdef LEON3
typedef struct _TIMERS_STRUCT {
	volatile unsigned int scaler_cnt; /* 0x00 */
	volatile unsigned int scaler_ld; /* 0x04 */
	volatile unsigned int config_reg; /* 0x08 */
	volatile unsigned int dummy1; /* 0x0C */
	volatile unsigned int timer_cnt1; /* 0x10 */
	volatile unsigned int timer_ld1; /* 0x14 */
	volatile unsigned int timer_ctrl1; /* 0x18 */
	volatile unsigned int dummy2; /* 0x1C */
	volatile unsigned int timer_cnt2; /* 0x20 */
	volatile unsigned int timer_ld2; /* 0x24 */
	volatile unsigned int timer_ctrl2; /* 0x28 */
} TIMERS_STRUCT;
#endif
#ifdef LEON2
typedef struct _TIMERS_STRUCT
{
	volatile unsigned int timer_cnt1; // 0x40
	volatile unsigned int timer_ld1;
	volatile unsigned int timer_ctrl1;
	volatile unsigned int wdog;
	volatile unsigned int timer_cnt2; //0x50
	volatile unsigned int timer_ld2;
	volatile unsigned int timer_ctrl2;
	volatile unsigned int dummy8;
	volatile unsigned int scaler_cnt; //0x60
	volatile unsigned int scaler_ld;
	volatile unsigned int dummy9;
	volatile unsigned int dummy10;
}TIMERS_STRUCT;
#endif
static TIMERS_STRUCT * timers = NULL;//(TIMERS_STRUCT *)(TIMERS_BASE);

#define MAX_QUANTITY_SYS_TIMERS 	0x20
typedef struct _SYS_TMR {
	volatile BOOL f_enable;
	volatile UINT32 id;
	volatile UINT32 load;
	volatile UINT32 cnt;
	volatile TIMER_FUNC handle;
} SYS_TMR;

static SYS_TMR sys_timers[MAX_QUANTITY_SYS_TIMERS];

volatile static UINT32 cnt_ms_sleep;//for sleep function
volatile static UINT32 cnt_sys_time;//quantity ms after start system


BOOL set_timer(UINT32 id, UINT32 ticks, TIMER_FUNC handle) {
	UINT32 i;
	for (i = 0; i < sizeof(sys_timers) / sizeof(sys_timers[0]); i++) {
		if (!sys_timers[i].f_enable) {
			sys_timers[i].handle = handle;
			sys_timers[i].load = ticks;
			sys_timers[i].cnt = ticks;
			sys_timers[i].id = id;
			sys_timers[i].f_enable = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

void close_timer(UINT32 id) {
	UINT32 i;
	for (i = 0; i < sizeof(sys_timers) / sizeof(sys_timers[0]); i++) {
		if (id == sys_timers[i].id) {
			sys_timers[i].f_enable = FALSE;
			sys_timers[i].handle = NULL;
		}
	}
}

static void inc_sys_timers() {
	UINT32 i;
	for (i = 0; i < sizeof(sys_timers) / sizeof(sys_timers[0]); i++) {
		if (sys_timers[i].f_enable) {
			if (!(sys_timers[i].cnt--)) {
				sys_timers[i].cnt = sys_timers[i].load;
				if (sys_timers[i].handle)
					sys_timers[i].handle(sys_timers[i].id);
			}
		}
	}
}

static void irq_func_tmr_1mS() {
	cnt_ms_sleep++;
	cnt_sys_time++;
	inc_sys_timers();
}

int timers_init() {
	APB_DEV apb_dev;
	int i;

	if (timers) {
		return -1;
	}
	for (i = 0; i < sizeof(sys_timers) / sizeof(sys_timers[0]); i++)
		sys_timers[i].f_enable = FALSE;

/*	if (-1 == capture_apb_dev(&apb_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_TIMER)) {
		printf("error capturing timer device");
		return -1; //error
	}
*/
	TRY_CAPTURE_APB_DEV (&apb_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_TIMER);
	timers = (TIMERS_STRUCT *)apb_dev.base;//(TIMERS_STRUCT *) (0x80000000 + ((apb_dev.ba_reg.addr) << 8)); // TODO 0x80000000
	timers->scaler_ld = TIMER_SCALER_VAL;
	timers->scaler_cnt = 0;
	timers->timer_cnt1 = 0;
	timers->timer_cnt2 = 0;

	timers->timer_ld1 = 0x002710;
	timers->timer_ld2 = 0;//0x027100;
	timers->timer_ctrl1 = 0xf;
	timers->timer_ctrl2 = 0x0;//disable
	//irq_set_handler(apb_dev.id_reg.irq, irq_func_tmr_1mS);

	irq_set_handler(apb_dev.irq, irq_func_tmr_1mS);
	cnt_sys_time = 0;

	return 0;
}

void sleep(int ms) {
	cnt_ms_sleep = 0;
	while (ms > cnt_ms_sleep)
		;
}

UINT32 get_sys_time() {
	return cnt_sys_time;
}

//TODO now save only one context
#define MAX_SAVE_CONTEXT 2
static SYS_TMR save_timers_buffer[MAX_QUANTITY_SYS_TIMERS][MAX_SAVE_CONTEXT];
static int save_context_number = 0;
int timers_context_save() {
	int context_number = 0;
	memcpy(save_timers_buffer[context_number], sys_timers, sizeof(sys_timers));
	return context_number;
}

int timers_context_restore(int context_number) {
	memcpy(sys_timers, save_timers_buffer[context_number], sizeof(sys_timers));
	return context_number;
}

