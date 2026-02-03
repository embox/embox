#ifndef OSAL_H_
#define OSAL_H_

#include <stdint.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/irq_lock.h>
#include <mem/sysmalloc.h>

struct os_thread_info;
struct os_queue;

#define pdFALSE			( ( uintptr_t ) 0 )
#define pdTRUE			( ( uintptr_t ) 1 )

#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )

extern int os_task_create(const char *name, void (*run)(void *), void *arg,
	    size_t stack_size, int priority, struct os_thread_info **info);
extern struct os_thread_info *os_get_current_task(void);
extern int os_task_notify_wait(uint32_t entry_bits, uint32_t exit_bits,
	    uint32_t *notif, int timeout);
extern void os_task_notify(struct os_thread_info *info, uint32_t value, uint32_t action);

extern int os_queue_create(size_t item_size, size_t max_items, struct os_queue **queue_p);
extern int os_queue_put(struct os_queue *q, const void *item, int timeout);
extern int os_queue_get(struct os_queue *q, void *item, int timeout);
extern int os_queue_peek(struct os_queue *q, void *item, int timeout);
extern int os_queue_messages_waiting(struct os_queue *q);
extern int os_queue_spaces_available(struct os_queue *q);

extern int os_mutex_create(struct mutex **m_p);

#define OS_STACK_WORD_SIZE      (sizeof(int))

#define OS_TASK                 struct os_thread_info*
#define OS_TASK_STATUS          int
#define OS_TASK_CREATE_SUCCESS  0
#define OS_TASK_NOTIFY_SUCCESS  0
#define OS_TASK_NOTIFY_FAIL     -1
#define OS_TASK_NOTIFY_NO_WAIT  0
#define OS_TASK_NOTIFY_FOREVER  ((unsigned) -1)
#define OS_TASK_NOTIFY_ALL_BITS 0xFFFFFFFF

#define OS_TASK_PRIORITY_LOWEST    0
#define OS_TASK_PRIORITY_NORMAL    128
#define OS_TASK_PRIORITY_HIGHEST   255

#define OS_MUTEX                struct mutex*
#define OS_MUTEX_TAKEN          1
#define OS_MUTEX_NOT_TAKEN      0
#define OS_MUTEX_CREATE_SUCCESS 0
#define OS_MUTEX_CREATE_FAILED  -1
#define OS_MUTEX_NO_WAIT        0
#define OS_MUTEX_FOREVER        ((unsigned) -1)

#define OS_EVENT_GROUP          void *

#define OS_QUEUE                struct os_queue*
#define OS_QUEUE_FULL           -ENOMEM
#define OS_QUEUE_EMPTY          -1
#define OS_QUEUE_OK             0
#define OS_QUEUE_NO_WAIT        0
#define OS_QUEUE_FOREVER        ((unsigned) -1)

#define OS_TIMER                void *
#define OS_TIMER_SUCCESS        pdPASS
#define OS_TIMER_FAIL           pdFAIL
#define OS_TIMER_FOREVER        ((unsigned) -1)

#define OS_BASE_TYPE            int
#define OS_UBASE_TYPE           int

#define OS_OK                   0
#define OS_FAIL                 -1

#define OS_TICK_TIME            int
#define OS_PERIOD_MS            0

#define OS_ASSERT(x) \
	if( ( x ) == 0 ) { do {} while(1); }

#define OS_NOTIFY_SET_BITS 1

/**** Task *****/

#define OS_GET_CURRENT_TASK()  \
	os_get_current_task()

#define OS_TASK_CREATE(name, task_func, arg, stack_size, priority, task) \
	os_task_create(name, task_func, arg, stack_size, priority, &(task))

#define OS_TASK_NOTIFY(task, value, action) \
	os_task_notify(task, value, action)

#define OS_TASK_NOTIFY_FROM_ISR(task, value, action) \
	os_task_notify(task, value, action)

#define OS_TASK_NOTIFY_WAIT(entry_bits, exit_bits, value, ticks_to_wait) \
	os_task_notify_wait(entry_bits, exit_bits, value, ticks_to_wait)

/**** Critical ****/

#define OS_ENTER_CRITICAL_SECTION() \
	irq_lock()

#define OS_LEAVE_CRITICAL_SECTION() \
	irq_unlock()

#define OS_LEAVE_CRITICAL_SECTION_FROM_ISR(a)

/**** Queue *****/

#define OS_QUEUE_CREATE(queue, item_size, max_items) \
	os_queue_create(item_size, max_items, &(queue))

#define OS_QUEUE_PUT(queue, item, timeout)    \
	os_queue_put(queue, item, timeout)

/* FIXME */
#define OS_QUEUE_PUT_FROM_ISR(queue, item)    \
	os_queue_put(queue, item, 0)

#define OS_QUEUE_GET(queue, item, timeout)    \
	os_queue_get(queue, item, timeout)

#define OS_QUEUE_PEEK(queue, item, timeout) \
	os_queue_peek(queue, item, timeout)

#define OS_QUEUE_MESSAGES_WAITING(queue) \
	os_queue_messages_waiting(queue)

// Instead of uxQueueSpacesAvailable
#define OS_QUEUE_SPACES_AVAILABLE(queue) \
	os_queue_spaces_available(queue)

/**** Mutex *****/

#define OS_MUTEX_CREATE(mutex) \
	os_mutex_create(&(mutex))

#define OS_MUTEX_GET(mutex, timeout) \
	mutex_lock(mutex)

#define OS_MUTEX_PUT(mutex) \
	mutex_unlock(mutex)

#include <time.h>
#include <kernel/time/time.h>
#include <kernel/time/sys_timer.h>
/**** Timer ****/
extern struct sys_timer *os_timer_create(int period, int reload, void *handler );
extern int os_timer_start(struct sys_timer * t );
extern int os_timer_stop(struct sys_timer * t );
extern int os_timer_change_period(struct sys_timer * t, int period );

#define OS_TIMER_CREATE(name, period, reload, timer_id, callback) \
			os_timer_create(period, reload, callback);

#define OS_TIMER_START(timer, timeout) \
		os_timer_start(timer)

#define OS_TIMER_STOP(timer, timeout) \
		os_timer_stop(timer)

#define OS_TIMER_START_FROM_ISR(timer) \
		os_timer_start(timer)

#define OS_TIMER_STOP_FROM_ISR(timer) \
		os_timer_stop(timer)

#define OS_TICKS_2_MS(ticks) \
		jiffies2ms(ticks)

/**
 * \brief Convert from ms to OS ticks
 *
 * \param [in] ms milliseconds to convert
 *
 * \return value in OS ticks
 *
 */
#define OS_MS_2_TICKS(ms) ms2jiffies(ms)

#define OS_TIME_TO_TICKS(ms) \
		ms2jiffies(ms)

#define OS_GET_TICK_COUNT() \
		clock()

#define OS_TIMER_CHANGE_PERIOD(timer, period, timeout) \
		os_timer_change_period(timer, period)

#define OS_TIMER_CHANGE_PERIOD_FROM_ISR(timer, period) \
		 os_timer_change_period(timer, period)

/**** Malloc ****/

#define OS_MALLOC(size)    sysmalloc(size)
#define OS_FREE(addr)      sysfree(addr)
#define OS_FREE_FUNC       sysfree


/**** Events ************/

#define OS_EVENT_GROUP_CREATE() NULL

#define OS_EVENT_GROUP_WAIT_BITS(event_group, bits_to_wait, clear_on_exit, wait_for_all, timeout)


#define OS_EVENT_GROUP_SET_BITS(event_group, bits_to_set)


#define OS_EVENT_GROUP_SET_BITS_FROM_ISR(event_group, bits_to_set) \
				OS_FAIL

#define OS_EVENT_GROUP_CLEAR_BITS(event_group, bits_to_clear)

#define OS_EVENT_GROUP_CLEAR_BITS_FROM_ISR(event_group, bits_to_clear)

#define OS_EVENT_GROUP_GET_BITS(event_group) 0

#define OS_EVENT_GROUP_GET_BITS_FROM_ISR(event_group) 0

/*************************** TODO PWR DEPS ****************************/

#define configUSE_TICKLESS_IDLE    0

static inline OS_MUTEX xSemaphoreCreateMutex(void) {
	return NULL;
}

#define OS_EVENT void *

#define OS_EVENT_SIGNALED  1

static inline int evt_wait(void) {
	return 0;
}

#define OS_EVENT_WAIT(mutex, timeout)  evt_wait()

#define OS_EVENT_FOREVER   (-1u)

#define OS_EVENT_SIGNAL(mutex)

#define OS_EVENT_CREATE(event)
#define OS_ENTER_CRITICAL_SECTION_FROM_ISR(event)
#define OS_EVENT_SIGNAL_FROM_ISR(event)

#define configPOST_SLEEP_PROCESSING()
#define configPRE_STOP_PROCESSING()
#define configPRE_SLEEP_PROCESSING(x)

#define configPRE_IDLE_ENTRY( x )               /*cm_lower_all_clocks()*/
#define configPOST_IDLE_ENTRY( x )              /*cm_restore_all_clocks()*/

#define configSYSTICK_CLOCK_HZ                  32000
#define configTICK_RATE_HZ                      500
#define TICK_PERIOD   (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ)

/*************************** TODO PWR DEPS ****************************/

#endif /* OSAL_H */
