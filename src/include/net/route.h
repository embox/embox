/**
 * @file
 * @brief Definitions for the IP router.
 * @details no cache routing.
 *
 * @date 16.11.09
 * @author Nikolay Korotky
 */
#ifndef ROUTE_H_
#define ROUTE_H_

#include <types.h>
#include <net/skbuff.h>
#include <net/ip.h>

/**
 * Routing table entry.
 */
typedef struct rt_entry {
	net_device_t *dev;
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
#endif
#define RTF_REJECT      0x0200          /* Reject route                 */

extern int ip_rt_init(void);

/**
 * Add new route to table.
 * @param dev Iface
 * @param dst Destination network
 * @param mask Genmask
 * @param gw Gateway
 */
extern int rt_add_route(net_device_t *dev, in_addr_t dst,
				in_addr_t mask, in_addr_t gw, int flags);

/**
 * Remove route from table.
 * @param dev Iface
 * @param dst Destination network
 * @param mask Genmask
 * @param gw Gateway
 */
extern int rt_del_route(net_device_t *dev, in_addr_t dst,
				in_addr_t mask, in_addr_t gw);

/**
 * Rebuild sk_buff according to appropriated route.
 * @param skbuff
 */
extern int ip_route(sk_buff_t *skbuff);

/**< iterators */
extern rt_entry_t *rt_fib_get_first(void);
extern rt_entry_t *rt_fib_get_next(void);

#endif /* ROUTE_H_ */
