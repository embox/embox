/*
* libcoap_embox_compat.h
*/

#ifndef LIBCOAP_EMBOX_COMPAT_H_
#define LIBCOAP_EMBOX_COMPAT_H_


#define IN_CLASSD(i) (((long)(i) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(i) IN_CLASSD(i)

#include <netinet/in.h>

#endif /* LIBCOAP_EMBOX_COMPAT_H_ */
