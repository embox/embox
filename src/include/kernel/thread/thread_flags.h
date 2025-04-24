/**
 * @file
 * @brief
 *
 * @date 08.04.14
 * @author Ilia Vaprol
 */

#ifndef THREAD_FLAGS_H_
#define THREAD_FLAGS_H_

#define THREAD_FLAG_DETACHED             (0x1 << 1) /**< Initially detached. */
#define THREAD_FLAG_JOINABLE             (0x0 << 1) /**< Initially not detached. */

/** Create thread with parent priority */
#define THREAD_FLAG_PRIORITY_INHERIT     (0x1 << 2)
#define THREAD_FLAG_PRIORITY_EXPLICIT    (0x0 << 2)
/** Create thread with decremented priority */
#define THREAD_FLAG_PRIORITY_LOWER       (0x1 << 3)
/** Create thread with incremented priority */
#define THREAD_FLAG_PRIORITY_HIGHER      (0x1 << 4)

/**< Create a new thread without launching */
#define THREAD_FLAG_SUSPENDED            (0x1 << 5)
/** Create a new thread without attaching to a task. */
#define THREAD_FLAG_NOTASK               (0x1 << 6)

#endif /* THREAD_FLAGS_H_ */
