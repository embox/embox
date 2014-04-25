/**
 * @file
 * @brief Definitions for the IP router.
 * @details no cache routing.
 *
 * @date 16.11.09
 * @author Nikolay Korotky
 */

#ifndef NET_L3_ROUTE_H_
#define NET_L3_ROUTE_H_

#include <stdint.h>
#include <net/skbuff.h>
#include <net/l3/ipv4/ip.h>
#include <stddef.h>
#include <net/netdevice.h>

struct net_device;

/**
 * Routing table entry.
 */
typedef struct rt_entry {
	struct net_device *dev;
	in_addr_t    rt_dst;
	uint32_t     rt_flags;
	in_addr_t    rt_mask;
	in_addr_t    rt_gateway;
} rt_entry_t;

/**< Flags */
#define RTF_UP          0x0001          /* route usable                 */
#define RTF_GATEWAY     0x0002          /* destination is a gateway     */
#if 0
#define RTF_HOST        0x0004          /* host entry (net otherwise)   */
#define RTF_REINSTATE   0x0008          /* reinstate route after tmout  */
#define RTF_DYNAMIC     0x0010          /* created dyn. (by redirect)   */
#define RTF_MODIFIED    0x0020          /* modified dyn. (by redirect)  */
#define RTF_MTU         0x0040          /* specific MTU for this route  */
#define RTF_MSS         RTF_MTU         /* Compatibility :-(            */
#define RTF_WINDOW      0x0080          /* per route window clamping    */
#define RTF_IRTT        0x0100          /* Initial round trip time      */
#define RTF_REJECT      0x0200          /* Reject route                 */
#endif

/**
 * Add new route to table.
 * @param dev Iface
 * @param dst Destination network
 * @param mask Genmask
 * @param gw Gateway
 */
extern int rt_add_route(struct net_device *dev, in_addr_t dst,
				in_addr_t mask, in_addr_t gw, int flags);

/**
 * Remove route from table.
 * @param dev Iface
 * @param dst Destination network
 * @param mask Genmask
 * @param gw Gateway
 */
extern int rt_del_route(struct net_device *dev, in_addr_t dst,
				in_addr_t mask, in_addr_t gw);

/**
 * Rebuild sk_buff according to appropriated route.
 * @param skbuff
 * @param route - take this route as best match. Or NULL to define it
 */
extern int ip_route(sk_buff_t *skb, struct net_device *wanna_dev,
		struct rt_entry *suggested_route);

/**
 * Get IP address of the next machine after routing
 * After this new_addr will be equal to source_addr or to gateway for this IP
 * @param ip address
 * @param pointer to result
 * @return error code
 */
extern int rt_fib_route_ip(in_addr_t source_addr, in_addr_t *new_addr);

/**
 * Get IP address of local interface from which packet would be sent
 * After this new_addr will be equal to interface address
 * @param ip address
 * @param pointer to result
 * @return error code
 */
extern int rt_fib_source_ip(in_addr_t dst, struct net_device *dev,
		in_addr_t *out_src);

extern int rt_fib_out_dev(in_addr_t dst, const struct sock *sk,
		struct net_device **out_dev);

/**
 * @param dst - ip address of destination
 * @param out_dev - device, from witch data will be send to dst.
 * @return pointer to best match entity for dst
 * @retval NULL if entity not found
 */
extern struct rt_entry* rt_fib_get_best(in_addr_t dst, struct net_device *out_dev);

/**
 * Get first element from route from table.
 * @return pointer to first entity
 * @retval NULL if entity not found
 */
extern struct rt_entry * rt_fib_get_first(void);

/**
 * Get next element from route from table uses
 * pointer to previous entry
 * Use this function after rt_fib_get_first
 * @param entry pointer to previous element
 * @return pointer to next entity
 * @retval NULL if entity more not found
 */
extern struct rt_entry * rt_fib_get_next(struct rt_entry *entry);

#endif /* NET_L3_ROUTE_H_ */
