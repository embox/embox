/**
 * @file
 * @brief Implementation of ARP Cache
 *
 * @date 12.08.2011
 * @author Ilia Vaprol
 */

#ifndef NET_NEIGHBOUR_H_
#define NET_NEIGHBOUR_H_

#include <net/if_ether.h>
#include <net/in.h>
#include <net/inetdevice.h>
#include <stdint.h>

/**
 * ARP Cache entity
 */
typedef struct neighbour {
	uint8_t hw_addr[ETH_ALEN];    /* MAC address, corresponding ip_addr */
	struct in_device *if_handler; /* Local interface */
	in_addr_t ip_addr;            /* IP address of detonation machine */
	uint8_t flags;                /* Status of entity (ATF_COM or ATF_PERM) */
} neighbour_t;

/**
 * Status flag
 */
#define ATF_COM     0x02 /* Completed entity
                          * It will be removed from cache
                          * after NEIGHBOUR_TIMEOUT ticks */
#define ATF_PERM    0x04 /* Permanent entry
                          * It will be saved in cache, until the
                          * neighbour_delete() function hasn't been called */

/**
 * After that time, an unused entry is deleted from the arp cache.
 * RFC1122 recommends set it to 60*HZ, if your site uses proxy arp
 * and dynamic routing.
 */
#define NEIGHBOUR_TIMEOUT        60000

/**
 * How often is ARP cache checked for expire.
 */
#define NEIGHBOUR_CHECK_INTERVAL 10000

/**
 * Timeout between repeatable ARP requests
 * RFC1122: Throttles ARPing, as per 2.3.2.1. (MUST)
 * The recommended minimum timeout is 1 second per destination.
 */
#define NEIGHBOUR_RES_TIME       1000

/**
 * This function search entry from arp table if can
 * @param in_dev (handler of inet_dev struct)
 * @param destination ip address
 * @return hardware address if can else NULL
 */
extern uint8_t * neighbour_lookup(struct in_device *if_handler, in_addr_t ip_addr);

/**
 * This function add entry in neighbour (rewrite)
 * @param in_dev (handler of inet_dev struct) which identificate network interface where address can resolve
 * @param ip_addr destination IP address
 * @param hw_addr destination MAC
 * @param ATF_COM or ATF_PERM
 * @return error code
 * @retval ENOERR
 * @retval -EINVAL
 * @retval -ENOMEM
 */
extern int neighbour_add(struct in_device *if_handler, in_addr_t ip_addr, uint8_t *hw_addr, uint8_t flags);

/**
 * This function delete entry from neighbour if can
 * @param in_dev (handler of inet_dev struct) which identificate network interface where address can resolve
 * @param ip_addr destination IP address
 * @return error code
 * @retval ENOERR (if deleted or entity not found)
 * @retval -EINVAL
 */
extern int neighbour_delete(struct in_device *if_handler, in_addr_t ip_addr);

/**
 * This function is the first step to get all valid neighbour entities
 * Used before neighbour_get_next() function
 * @return pointer to first neighbour entity if arp cache not empty else NULL
 */
extern struct neighbour * neighbour_get_first(void);

/**
 * This function gets next entity from ARP cache
 * Used after neighbour_get_first() function
 * @param pentity pointer to current entity in the cache
 * @return pointer to neighbour entity if exist, else NULL
 * @retval ENOERR that means next entity has been written in memory pointed by pentity
 * @retval -EINVAL if pointer is NULL or reached the end of the list
 */
extern int neighbour_get_next(struct neighbour **pentity);

#endif /* NET_NEIGHBOUR_H_ */
