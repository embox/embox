/**
 * @file
 * @brief
 *
 * @date 20.10.13
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_TCP_H_
#define NET_LIB_TCP_H_

#include <netinet/in.h>
#include <stdint.h>

/**
 * Prototypes
 */
struct iphdr;
struct ip6hdr;
struct tcphdr;

/**
 * Build TCP header
 */
extern void tcp_build(struct tcphdr *tcph, in_port_t dst_prt,
		in_port_t src_prt, uint8_t data_off, uint16_t window);

/**
 * Set TCP seq field
 */
extern void tcp_set_seq_field(struct tcphdr *tcph,
		uint32_t seq);

/**
 * Set TCP ack and ack_seq field
 */
extern void tcp_set_ack_field(struct tcphdr *tcph,
		uint32_t ack_seq);

/**
 * Set TCP check field
 */
extern void tcp4_set_check_field(struct tcphdr *tcph,
		const struct iphdr *iph);
extern void tcp6_set_check_field(struct tcphdr *tcph,
		const struct ip6hdr *ip6h);
extern void tcp_set_check_field(struct tcphdr *tcph,
		const void *nhhdr);

/**
 * Calculate TCP data length
 */
extern size_t tcp4_data_length(const struct tcphdr *tcph,
		const struct iphdr *iph);
extern size_t tcp6_data_length(const struct tcphdr *tcph,
		const struct ip6hdr *ip6h);
extern size_t tcp_data_length(const struct tcphdr *tcph,
		const void *nhhdr);

/**
 * Calculate TCP sequance length
 */
extern size_t tcp4_seq_length(const struct tcphdr *tcph,
		const struct iphdr *iph);
extern size_t tcp6_seq_length(const struct tcphdr *tcph,
		const struct ip6hdr *ip6h);
extern size_t tcp_seq_length(const struct tcphdr *tcph,
		const void *nhhdr);

#endif /* NET_LIB_TCP_H_ */
