#ifndef _UAPI__IF_TUN_H
#define _UAPI__IF_TUN_H

#include <net/l2/ethernet.h>
#include <sys/ioctl.h>

#define MAX_TUNTAP_DEVICES 16

/* Ioctl defines */
#define TUNSETIFF     _IOW('T', 202, int) 
#define TUNGETIFF      _IOR('T', 210, unsigned int)

/* TUNSETIFF ifr flags */
#define IFF_TUN		0x0001
#define IFF_TAP		0x0002

#endif /* _UAPI__IF_TUN_H */

