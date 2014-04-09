#ifndef IPERF_UNISTD_H_
#define IPERF_UNISTD_H_

#include <sys/types.h>

pid_t setsid(void);

#include_next <unistd.h>

#endif /* IPERF_UNISTD_H_ */

