/**
 * \file in.h
 * \date 29.06.09
 * \author sikmir
 */

#ifndef IN_H_
#define IN_H_

// IPv4 AF_INET sockets:

struct in_addr {
        unsigned long    s_addr;
};

struct sockaddr_in {
	short            sin_family;   /* e.g. AF_INET */
        unsigned short   sin_port;     /* e.g. htons(3490) */
        struct in_addr   sin_addr;     /* see struct in_addr, below */
        char             sin_zero[8];  /* zero this if you want to */
};

unsigned long inet_addr(const unsigned char *cp) {
	unsigned long tmp = 0x00000000;
	int i;
	for(i=0; i<4; i++) {
		tmp += ((0xFF & cp[i]) << (3-i)*8);
	}
	return tmp;
}

#define htons(n)            n

/* Address to accept any incoming messages. */
#define INADDR_ANY          ((unsigned long int) 0x00000000)

/* Address to send to all hosts. */
#define INADDR_BROADCAST    ((unsigned long int) 0xffffffff)

/* Address to loopback in software to local host.  */
#define INADDR_LOOPBACK     ((unsigned long int) 0x7f000001)   /* 127.0.0.1   */

#endif /* IN_H_ */
