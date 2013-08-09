/**
 * @file
 * @brief
 *
 * @date 25.07.2013
 * @author Andrey Gazukin
 */

#ifndef __LINUX_STAT_H_
#define __LINUX_STAT_H_

#include <sys/stat.h>

#define S_IRUGO (S_IRUSR|S_IRGRP|S_IROTH)
#define S_IWUGO (S_IWUSR|S_IWGRP|S_IWOTH)
#define S_IXUGO (S_IXUSR|S_IXGRP|S_IXOTH)
#define S_IRWXUGO (S_IRWXU|S_IRWXG|S_IRWXO)

#endif /* STAT_H_ */
