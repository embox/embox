/**
 * @file
 * @brief
 *
 * @date 01.08.2013
 * @author Andrey Gazukin
 */

#ifndef __LINUX_WORKQUEUE_H__
#define __LINUX_WORKQUEUE_H__

/* We don't do this yet */
struct work_struct { } ;

#define INIT_WORK(x,y,z) /* */
#define schedule_work(x) do { } while(0)
#define flush_scheduled_work() do { } while(0)

#endif /* __LINUX_WORKQUEUE_H__ */
