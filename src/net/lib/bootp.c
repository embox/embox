/**
 * @file
 * @brief BOOTP implementation (client)
 *
 * @date 01.08.12
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <assert.h>
#include <net/lib/bootp.h>
#include <string.h>

const unsigned char dhcp_magic_cookie[4] = { 0x63, 0x82, 0x53, 0x63 };

int bootp_build(struct bootphdr *bph, unsigned char opcode,
		unsigned char hw_type, unsigned char hw_len,
		unsigned char *hw_addr) {
	if ((bph == NULL) || (hw_addr == NULL)) {
		return -EINVAL;
	}

	memset(bph, 0, sizeof *bph);

	bph->op = opcode;
	bph->htype = hw_type;
	bph->hlen = hw_len;
	memcpy(&bph->chaddr[0], hw_addr, hw_len);
	memset(&bph->vend[0], TAG_END, sizeof bph->vend);
	static_assert(sizeof bph->vend >= sizeof dhcp_magic_cookie);
	memcpy(&bph->vend[0], &dhcp_magic_cookie[0],
			sizeof dhcp_magic_cookie);

	return 0;
}

int bootp_valid(struct bootphdr *bph, unsigned char opcode) {
	if (bph == NULL) {
		return -EINVAL;
	}

	if ((bph->op != opcode) || (0 != memcmp(&bph->vend[0],
				&dhcp_magic_cookie[0], sizeof dhcp_magic_cookie))) {
			return -EINVAL;
	}

	return 0;
}
