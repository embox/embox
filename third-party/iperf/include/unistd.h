#ifndef IPERF_UNISTD_H_
#define IPERF_UNISTD_H_

#include_next <unistd.h>

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

pid_t setsid(void);

__END_DECLS

#endif /* IPERF_UNISTD_H_ */

