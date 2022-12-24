/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.12.22
 */

#ifndef REMOTE_H_
#define REMOTE_H_

#include <stddef.h>

/**
 * @brief Read request from remote debugger
 * @retval On success: number of bytes read
 * @retval On error: -errno
 * 
 */
extern int remote_read(void *buf, size_t count);

/**
 * @brief Write response to remote debugger
 * @retval On success: number of bytes written
 * @retval On error: -errno
 */
extern int remote_write(const void *buf, size_t count);

/**
 * @brief Open connection to remote debugger
 * @retval On success: 0
 * @retval On error: -errno
 */
extern int remote_open(const char *host, const char *port);

/**
 * @brief Close connection to remote debugger
 */
extern void remote_close(void);

#endif /* REMOTE_H_ */
