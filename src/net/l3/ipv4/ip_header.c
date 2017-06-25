/**
 * @file
 *
 * @date Jun 19, 2014
 * @author: Anton Bondarev
 */
#include <stdint.h>
#include <fs/idesc.h>
#include <net/sock.h>
#include <net/l3/ipv4/ip.h>
#include <security/security.h>

#include <net/skbuff.h>

int ip_header_size(struct sock *sock) {
	int header_size = IP_MIN_HEADER_SIZE;
	char label[32];

	if (0 > security_sock_label(sock, label, sizeof(label))) {
		return header_size;
	}

	header_size += 12;

	return header_size;
}

uint16_t smac_label_to_secure_level(const char *label) {

	if (0 == strncmp(label, "secret", sizeof("secret"))) {
		return htons(0x8000);
	}
	if (0 == strncmp(label, "confidentially", sizeof("confidentially"))) {
		return htons(0x4000);
	}
	if (0 == strncmp(label, "service", sizeof("service"))) {
		return htons(0x2000);
	}
	if (0 == strncmp(label, "unclassified", sizeof("unclassified"))) {
		return htons(0x1000);
	}
	return htons(0x1000);
}

const char *smac_secure_level_to_label(uint16_t level) {
	switch(level) {
	case 0x8000:
		return "secret";
	case 0x4000:
		return "confidentially";
	case 0x2000:
		return "service";
	case 0x1000:
		return "unclassified";
	default:
		return "unclassified";
	}
}

int ip_header_make_secure(struct sock *sock, struct sk_buff *skb) {
	uint8_t *options;
	char label[32];
	uint16_t level;

	options = (unsigned char *)skb->nh.iph + skb->nh.iph->ihl * 4;
	options[0] = 0x80 | 0x00 | 0x02; /* copy | class = 0 | number = 2 */
	options[1] = 11; /* length = 11 = b00001011*/

	/* Security. 16 bits.
	 Specifies one of 16 levels of security.
	*/

	if (0 > security_sock_label(sock, label, sizeof(label))) {
		return -1;
	}
	level = smac_label_to_secure_level(label);
	options[2] = (uint8_t)(level >> 8 & 0xFF);
	options[3] = (uint8_t)(level & 0x00FF);

	/* Compartments. 16 bits.
	 * An all zero value is used when the information transmitted is not
	 * compartmented. Other values for the compartments field may be obtained
	 * from the Defense Intelligence Agency.
	 */
	options[4] = 0;
	options[5] = 0;

	/* Handling restrictions. 16 bits.
	 * The values for the control and release markings are alphanumeric digraphs
	 * and	are defined in the Defense Intelligence Agency Manual DIAM 65-19,
	 * "Standard Security Markings".
	 */
	options[6] = 0;
	options[7] = 0;

	/*
	 * Transmission Control Code. 24 bits.
	 * Provides a means to segregate traffic and define controlled communities of
	 * interest among subscribers. The TCC values are trigraphs, and are available
	 * from HQ DCA Code 530.
	 */

	options[8] = 0;
	options[9] = 0;
	options[10] = 0;

	skb->nh.iph->ihl += 12 / 4;
	/* because reserve 12 bytes */
	options[11] = 0;

	return 0;
}

uint16_t sock_get_secure_level(struct sock *sk) {
	char label[32];

	if (0 > security_sock_label(sk, label, sizeof(label))) {
		return 0;
	}

	return smac_label_to_secure_level(label);
}

