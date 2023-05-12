/**
 * @file
 * @brief
 *
 * @date 11.05.23
 * @author Aleksey Zhmulin
 */
#ifndef IPERF_SYS_SOCKET_H_
#define IPERF_SYS_SOCKET_H_

#include_next <sys/socket.h>
#undef MSG_PEEK
#undef MSG_DONTWAIT
#undef MSG_WAITALL

#endif /* IPERF_SYS_SOCKET_H_ */
