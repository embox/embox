// #include <arpa/inet.h>
#include <errno.h>
#include <netdb.h> 
// #include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE 	500
#define TEST_PORT	"1337"

extern int sock_connect(const char *nodename, const char *servname,
						struct addrinfo *hints);