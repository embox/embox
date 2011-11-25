/**
 * @date 25.11.2011
 */

#ifndef NET_IP_FRAGMENT_H
#define NET_IP_FRAGMENT_H

#include <types.h>
#include <util/list.h>
#include <util/slist.h>
#include <net/in.h>
#include <net/ip.h>

#define MAX_BUFS_CNT 0x10
#define MAX_HOLES_CNT MAX_BUFS_CNT * 0x10

struct hole {
	struct list_link next_hole;
	int begin;
	int end;
};

/**
 * Datagram receive buffer descriptor
 */
typedef struct dgram_buf {
	struct slist_link next_buf;		/* linked list pointers	*/
	in_addr_t		  saddr;
	in_addr_t		  daddr;
	uint16_t		  id;
	uint8_t		      protocol;
	uint8_t		      complete;
	struct list_link  holes;
	int				  len; /* total length of original datagram	*/
}dgram_buf_t;

#define dgram_buf_foreach_ptr(dgram_buf) \
	slist_foreach(dgram_buf, __extension__ ({   \
	extern struct slist __dgram_buf_list; &__dgram_buf_list; \
	}), next_buf)

#endif /* NET_IP_FRAGMENT_H */
