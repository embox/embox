/**
 * @file timers_helper.c
 */

#include <types.h>
#include <unistd.h>
#include <kernel/irq.h>
#include <kernel/timers.h>
#include <time.h>
#include <common.h> /*for array_len*/
#include <string.h>
#include <kernel/module.h>

DECLARE_INIT("sys_timers", sys_timers_init, INIT_SYSTEM_LEVEL);

typedef struct sys_tmr {
        volatile int        f_enable;
        volatile uint32_t   id;
        volatile uint32_t   load;
        volatile uint32_t   cnt;
        volatile TIMER_FUNC handle;
} sys_tmr_t;

static volatile uint32_t cnt_ms_sleep; /**< for sleep function */
static volatile uint32_t cnt_sys_time; /**< quantity ms after start system */

static sys_tmr_t sys_timers[_SC_TIMER_MAX];


static void set_sys_timer_enable (int num) {
        sys_timers[num].f_enable = 1;
}

static void set_sys_timer_disable (int num) {
        sys_timers[num].f_enable = 0;
}

int set_timer (uint32_t id, uint32_t ticks, TIMER_FUNC handle) {
        int i;
        for (i = 0; i < array_len (sys_timers); i++) {
                if (!sys_timers[i].f_enable) {
                        sys_timers[i].handle = handle;
                        sys_timers[i].load = ticks;
                        sys_timers[i].cnt = ticks;
                        sys_timers[i].id = id;
                        set_sys_timer_enable(i);
                        return 1;
                }
        }
        return 0;
}

void close_timer (uint32_t id) {
        int i;
        for (i = 0; i < array_len (sys_timers); i++) {
                if (id == sys_timers[i].id) {
                        sys_timers[i].f_enable = 0;
                        sys_timers[i].handle = NULL;
                }
        }
}

static void inc_sys_timers (void) {
        int i;
        for (i = 0; i < array_len (sys_timers); i++) {
                if (sys_timers[i].f_enable && !(sys_timers[i].cnt--)) {
                        sys_timers[i].cnt = sys_timers[i].load;
                        if (sys_timers[i].handle) {
                                sys_timers[i].handle(sys_timers[i].id);
                        }
                }
        }
}

static void irq_func_tmr_1mS(int irq_num, void *dev_id, struct pt_regs *regs) {
        unsigned long irq = local_irq_save();
        cnt_ms_sleep++;
        cnt_sys_time++;
        inc_sys_timers();
        local_irq_restore(irq);
}

static int sys_timers_init(void) {
        int i;
        cnt_sys_time = 0;
        for (i = 0; i < array_len(sys_timers); i++) {
                set_sys_timer_disable(i);
        }
        return timers_ctrl_init(irq_func_tmr_1mS);
}

int usleep(useconds_t usec) {
        unsigned int irq = local_irq_save();
        cnt_ms_sleep = 0;
        local_irq_restore(irq);

        while (cnt_ms_sleep < usec) {}
        return 0;
}

/*TODO now save only one timer context*/
#define MAX_SAVE_CONTEXT          2

static sys_tmr_t save_timers_buffer[_SC_TIMER_MAX][MAX_SAVE_CONTEXT];
#if 0
static int save_context_number = 0;
#endif

int timers_context_save () {
        int context_number = 0;
        memcpy(save_timers_buffer[context_number], sys_timers,
    						sizeof (sys_timers));
        return context_number;
}

int timers_context_restore (int context_number) {
        memcpy(sys_timers, save_timers_buffer[context_number],
    						sizeof (sys_timers));
        return context_number;
}

/*TODO timers_off why?*/
#if 0
void timers_off () {
        platform_timers_off ();
}
#endif
