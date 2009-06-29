/**
 * \file in.h
 * \date 29.06.09
 * \author sikmir
 */

#ifndef IN_H_
#define IN_H_

typedef unsigned short  sa_family_t;

struct in_addr {
        unsigned int  s_addr;
};

struct sockaddr_in {
        sa_family_t           sin_family;     /* Address family       */
        unsigned int          sin_port;       /* Port number          */
        struct in_addr        sin_addr;       /* Internet address     */
};

#endif /* IN_H_ */
