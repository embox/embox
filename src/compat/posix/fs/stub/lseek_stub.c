/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <util/log.h>

off_t lseek(int fd, off_t offset, int origin) {
	log_warning(">>> %s", __func__);
	return (off_t)-1;
}
