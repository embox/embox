/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_WAIT_DATA_H_
#define KERNEL_THREAD_WAIT_DATA_H_

#define WAIT_DATA_STATUS_NONE        0
#define WAIT_DATA_STATUS_WAITING     1
#define WAIT_DATA_STATUS_WAKED       2

struct wait_data {
	struct work work;

	int status;
	int result;
};

#endif /* KERNEL_THREAD_WAIT_DATA_H_ */
