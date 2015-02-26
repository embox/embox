#ifndef LUASOCKET_SYS_UN_H_
#define LUASOCKET_SYS_UN_H_

#include <sys/socket.h>

struct sockaddr_un {
	sa_family_t sun_family;
	char sun_path[64];
};

#endif /* LUASOCKET_SYS_UN_H_ */
