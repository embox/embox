#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "module/third_party/lib/nanomodbus.h"


#define UNUSED_PARAM(x) ((x) = (x))


// Connection management

int client_connection = -1;

void* connect_tcp(const char* address, const char* port) {
    struct addrinfo ainfo = {0};
    struct addrinfo* results;
    struct addrinfo* rp;
    int fd;

    ainfo.ai_family = AF_INET;
    ainfo.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo(address, port, &ainfo, &results);
    if (ret != 0)
        return NULL;

    for (rp = results; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1)
            continue;

        ret = connect(fd, rp->ai_addr, rp->ai_addrlen);
        if (ret == 0)
            break;

        close(fd);
    }

    freeaddrinfo(results);

    if (rp == NULL)
        return NULL;

    client_connection = fd;
    return &client_connection;
}


int server_fd = -1;
int client_read_fd = -1;
fd_set client_connections;

void close_tcp_server() {
    if (server_fd != -1) {
        close(server_fd);
        printf("Server closed\n");
    }
}


int create_tcp_server(const char* address, const char* port) {
    struct addrinfo ainfo = {0};
    struct addrinfo* results;
    struct addrinfo* rp;
    int fd = -1;

    ainfo.ai_family = AF_INET;
    ainfo.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo(address, port, &ainfo, &results);
    if (ret != 0)
        return -1;

    for (rp = results; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1)
            continue;

        ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
        if (ret != 0)
            return -1;

        ret = bind(fd, rp->ai_addr, rp->ai_addrlen);
        if (ret != 0) {
            close(fd);
            fd = -1;
            continue;
        }

        ret = listen(fd, 1);
        if (ret != 0) {
            close(fd);
            fd = -1;
            continue;
        }

        break;
    }

    freeaddrinfo(results);

    if (fd < 0) {
        return errno;
    }

    server_fd = fd;
    FD_ZERO(&client_connections);

    return 0;
}


void* server_poll(void) {
    fd_set read_fd_set;
    FD_ZERO(&read_fd_set);

    while (true) {
        read_fd_set = client_connections;
        FD_SET(server_fd, &read_fd_set);

        int ret = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);
        if (ret < 0)
            return NULL;

        for (int i = 0; i < FD_SETSIZE; ++i) {
            if (FD_ISSET(i, &read_fd_set)) {
                if (i == server_fd) {
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_size = sizeof(client_addr);

                    int client = accept(server_fd, (struct sockaddr*) &client_addr, &client_addr_size);
                    if (client < 0) {
                        fprintf(stderr, "Error accepting client connection from %s - %s\n",
                                inet_ntoa(client_addr.sin_addr), strerror(errno));
                        continue;
                    }

                    FD_SET(client, &client_connections);
                    printf("Accepted connection %d from %s\n", client, inet_ntoa(client_addr.sin_addr));
                }
                else {
                    client_read_fd = i;
                    return &client_read_fd;
                }
            }
        }
    }
}


void disconnect(void* conn) {
    int fd = *(int*) conn;
    FD_CLR(fd, &client_connections);
    close(fd);
    printf("Closed connection %d\n", fd);
}


// Read/write platform functions

int32_t read_fd_linux(uint8_t* buf, uint16_t count, int32_t timeout_ms, void* arg) {
    if (!arg) {
        return -1;
    }

    int fd = *(int*) arg;

    uint16_t total = 0;
    while (total != count) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        struct timeval* tv_p = NULL;
        struct timeval tv;
        if (timeout_ms >= 0) {
            tv_p = &tv;
            tv.tv_sec = timeout_ms / 1000;
            tv.tv_usec = (int64_t) (timeout_ms % 1000) * 1000;
        }

        int ret = select(fd + 1, &rfds, NULL, NULL, tv_p);
        if (ret == 0) {
            return total;
        }

        if (ret == 1) {
            ssize_t r = read(fd, buf + total, 1);
            if (r == 0) {
                disconnect(arg);
                return 0;
            }

            if (r < 0)
                return -1;

            total += r;
        }
        else
            return -1;
    }

    return total;
}


int32_t write_fd_linux(const uint8_t* buf, uint16_t count, int32_t timeout_ms, void* arg) {
    int fd = *(int*) arg;

    uint16_t total = 0;
    while (total != count) {
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(fd, &wfds);

        struct timeval* tv_p = NULL;
        struct timeval tv;
        if (timeout_ms >= 0) {
            tv_p = &tv;
            tv.tv_sec = timeout_ms / 1000;
            tv.tv_usec = (int64_t) (timeout_ms % 1000) * 1000;
        }

        int ret = select(fd + 1, NULL, &wfds, NULL, tv_p);
        if (ret == 0) {
            return 0;
        }

        if (ret == 1) {
            ssize_t w = write(fd, buf + total, count - total);
            if (w == 0) {
                disconnect(arg);
                return -1;
            }

            if (w <= 0)
                return -1;

            total += (int32_t) w;
        }
        else
            return -1;
    }

    return total;
}
