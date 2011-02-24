/**
 * @file
 * @brief Defines structure of threads and methods,
 * which allows working with them.
 *
 * @date 22.04.2010
 * @author Avdyukhin Dmitry
 *         - Initial implementation
 * @author Alina Kramar
 *         - Extracting internal implementation to separate header
 *         - Thread iteration code
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <hal/context.h>
#include <lib/list.h>
#include <queue.h>
#include <string.h>
#include <util/macro.h>
#include <util/guard.h>
#ifdef CONFIG_PP_ENABLE
# include <kernel/pp.h>
#endif

#include <impl/kernel/thread.h>

typedef __thread_id_t thread_id_t;
typedef __thread_priority_t thread_priority_t;

typedef int msg_t;
typedef void * msg_data_t;

/**
 * Structure, describing messages, sent to threads.
 */
struct message {
	/* Message type. */
	msg_t type;
	/* Information in message. */
	msg_data_t data;
	/* Node in queue of messages. */
	struct list_head list;
};

/**
 * Thread, which makes nothing.
 * Is used to be working when there is no another process.
 */
extern struct thread *idle_thread;

/**
 * Thread, which is working now.
 */
extern struct thread *current_thread;

struct pprocess;
/**
 * Structure, describing threads.
 */
struct thread;

#define thread_foreach(t) __thread_foreach(t)

extern struct thread *thread_get_by_id(thread_id_t id);

/**
 * Creates new thread
 *
 * @param run function executed in created thread.
 * @param stack_address address of thread's stack.
 * @return pointer to new thread if all parameters are correct.
 * @return NULL if one of parameters is NULL or all places for threads are occupied.
 */
extern struct thread *thread_create(void(*run)(void), void *stack_address);

/**
 * Starts a thread.
 */
extern void thread_start(struct thread *thread);

/**
 * Changes thread's priority.
 */
extern void thread_change_priority(struct thread *thread, int new_priority);

/**
 * Stops chosen thread.
 * Deletes previous zombie.
 * Makes it a zombie.
 */
extern int thread_stop(struct thread *stopped_thread);

/**
 * Switches context to another thread.
 * Currently working thread leaves CPU for some time.
 */
extern void thread_yield(void);

/**
 * Send message to thread.
 *
 * @param message sent message
 * @param thread thread to receive message
 */
extern void msg_send(struct message *message, struct thread *thread);

/**
 * Allows thread to receive a message.
 *
 * @param thread thread, which wants to take a message.
 * @return received message (wait until there will be such a message).
 */
extern struct message *msg_receive(void);

/**
 * Allots memory for new message to send to thread.
 *
 * @return pointer to new message.
 */
extern struct message *msg_new(void);

/**
 * Free memory from the message.
 *
 * @param message deleted message.
 * @return -1 if message is NULL
 * @return 0 otherwise.
 */
extern int msg_erase(struct message *message);

#endif /* THREAD_H_ */
