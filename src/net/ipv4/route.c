/**
 * @file route.c
 *
 * @brief implementation of the IP router.
 * @date 16.11.09
 * @author Nikolay Korotky
 */

#include <kernel/module.h>
#include <net/route.h>
#include <net/skbuff.h>
#include <net/arp.h>
#include <lib/inet/netinet/in.h>

#define RT_TABLE_SIZE 16

/**
 * NOTE: Linux route uses 3 structs for routing:
 *    + Forwarding Information Base (FIB)
 *    - routing cache (256 chains)
 *    - neibour table
 */
static rt_entry_t rt_table[RT_TABLE_SIZE];

int __init ip_rt_init(void) {
	devinet_init();
}

int rt_add_route(net_device_t *dev, in_addr_t dst,
        		    in_addr_t mask, in_addr_t gw, int flags) {
	int i;
        for (i = 0; i < RT_TABLE_SIZE; i++) {
                if (!(rt_table[i].rt_flags & RTF_UP)) {
            		rt_table[i].dev        = dev;
            		rt_table[i].rt_dst     = dst;
            		rt_table[i].rt_mask    = mask;
            		rt_table[i].rt_gateway = gw;
            		rt_table[i].rt_flags   = RTF_UP|flags;
                        return 0;
                }
        }
        return -1;
}

int rt_del_route(net_device_t *dev, in_addr_t dst,
			    in_addr_t mask, in_addr_t gw) {
	int i;
	for(i = 0; i < RT_TABLE_SIZE; i++) {
	        if ((rt_table[i].rt_dst == dst || INADDR_ANY == dst) &&
	    	    (rt_table[i].rt_mask == mask || INADDR_ANY == mask) &&
	    	    (rt_table[i].rt_gateway == gw || INADDR_ANY == gw) ) {
	                rt_table[i].rt_flags &= ~RTF_UP;
	                return 0;
	        }
	}
	return -1;
}

int ip_route(sk_buff_t *skbuff) {
	int i;
	for(i = 0; i < RT_TABLE_SIZE; i++) {
		if (rt_table[i].rt_flags & RTF_UP) {
			if( (skbuff->nh.iph->daddr & rt_table[i].rt_mask) == rt_table[i].rt_dst) {
				skbuff->dev = rt_table[i].dev;
				arp_find(skbuff, rt_table[i].rt_gateway);
				return 0;
			}
		}
	}
	return -1;
}

static int rt_iter;

rt_entry_t *rt_fib_get_first() {
	for(rt_iter = 0; rt_iter < RT_TABLE_SIZE; rt_iter++) {
    		if (rt_table[rt_iter].rt_flags & RTF_UP) {
            		rt_iter++;
            		return &rt_table[rt_iter - 1];
    		}
        }
        return NULL;
}

rt_entry_t *rt_fib_get_next() {
	for(; rt_iter < RT_TABLE_SIZE; rt_iter++) {
    		if (rt_table[rt_iter].rt_flags & RTF_UP) {
                	rt_iter++;
                        return &rt_table[rt_iter - 1];
                }
        }
        return NULL;
}
