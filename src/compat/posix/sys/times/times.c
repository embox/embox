/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.05.2014
 */

#include <sys/times.h>

#include <kernel/task.h>
#include <hal/clock.h>

clock_t times (struct tms *__buffer) {
	//DPRINT();
	__buffer->tms_cstime = __buffer->tms_cutime = 0;
	__buffer->tms_stime = task_get_clock(task_self());
	__buffer->tms_utime = 0;

	return clock_sys_ticks();
}


