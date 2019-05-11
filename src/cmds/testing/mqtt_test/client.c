#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
	char buf[] = "test";
	int out_size = 4;

	struct sockaddr_in serverAddr;
    socklen_t addr_size;
	int fd;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("Cannot retrieve socket\n");
        return -1;
    }

	int err;
	int broadcast = 1;
	err = setsockopt(fd, SOL_SOCKET, SO_BROADCAST,
			&broadcast, sizeof(broadcast));
	if (err) {
		printf("Cannot set option 1\n");
		return -1;
	}

	int enable = 1;
	err = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
			&enable, sizeof(int));
	if (err) {
		printf("Cannot set option 2\n");
		return -1;
	}

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = (in_port_t) htons(1337);
    serverAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    addr_size = sizeof(serverAddr);

    ssize_t rc = sendto(fd, buf, out_size, 0,
			(struct sockaddr *)&serverAddr, addr_size);

	printf("rc == %i\nout_size == %i\n", rc, out_size);
    if (rc != out_size) printf("Sending error");
	
	return 0;
}
