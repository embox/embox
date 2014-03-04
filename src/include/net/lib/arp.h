/**
 * @file
 * @brief
 *
 * @date 20.01.14
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_ARP_H_
#define NET_LIB_ARP_H_

#include <stdint.h>

/**
 * Prototypes
 */
struct arpbody;
struct arphdr;

/**
 * Build ARP header
 */
extern void arp_build(struct arphdr *arph, uint16_t hrd,
		uint16_t pro, uint8_t hln, uint8_t pln, uint16_t op,
		const void *sha, const void *spa, const void *tha,
		const void *tpa);

/**
 * Make ARP body
 */
extern void arp_make_body(struct arphdr *arph,
		struct arpbody *out_arpb);

#endif /* NET_LIB_ARP_H_ */
