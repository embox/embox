/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 */
#include <unistd.h>


extern pid_t kfork(void);

pid_t fork(void) {
	return kfork();
}
