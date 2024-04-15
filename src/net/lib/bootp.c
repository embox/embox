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
#include <string.h>
#include <stdint.h>

#include <util/err.h>

#include <net/lib/bootp.h>

static const uint8_t dhcp_magic_cookie[4] = { 0x63, 0x82, 0x53, 0x63 };
static const uint8_t dhcp_discovery_options[] = {
		TAG_DHCP_MESS_TYPE, 1, BOOTPREQUEST, /* required by busybox */
		TAG_DHCP_PARM_REQ_LIST, 0, /* request option all option list from server */
		TAG_END /* the end */
};

static const uint8_t dhcp_msg_type[] = {
		TAG_DHCP_MESS_TYPE, 1
};

static const uint8_t dhcp_offer_lease_time[] = {
		TAG_DHCP_LEASE_TIME, 4,
		0, 0, 0x2, 0x58 /* 10 min */
};

static const uint8_t dhcp_offer_server_id[] = {
		TAG_DHCP_SERVER_ID, 4
};

static const uint8_t dhcp_offer_netmask[] = {
		TAG_SUBNET_MASK, 4,
		255, 255, 255, 0
};
#if 0
static const uint8_t dhcp_offer_router[] = {
		TAG_GATEWAY, 4
};
#endif

static void *bootp_add_option(void *ptr, uint8_t option[], int size) {
	memcpy(ptr, option, size);

	return ptr + size;
}

int bootp_build_offer(struct bootphdr *bph, uint32_t xid, uint8_t msg_type,
		uint8_t hw_type, uint8_t hw_len, uint8_t *hw_addr,
		in_addr_t client_ip_addr, in_addr_t server_ip_addr) {
	uint8_t *ptr;

	memset(bph, 0, sizeof(struct bootphdr));

	bph->op = BOOTPREPLY;
	bph->htype = hw_type;
	bph->hlen = hw_len;
	memcpy(&bph->chaddr[0], hw_addr, hw_len);

	memcpy(&bph->xid, &xid, sizeof(bph->xid));
	memcpy(&bph->yiaddr, &client_ip_addr, sizeof(client_ip_addr));
	memcpy(&bph->giaddr, &server_ip_addr, sizeof(server_ip_addr));

	ptr = bootp_add_option(&bph->vend[0], (uint8_t *)&dhcp_magic_cookie[0], sizeof(dhcp_magic_cookie));
	/* setup options */
	ptr = bootp_add_option(ptr, (uint8_t *)dhcp_msg_type, sizeof(dhcp_msg_type));
	ptr = bootp_add_option(ptr, &msg_type, sizeof(msg_type));

	ptr = bootp_add_option(ptr, (uint8_t *)dhcp_offer_server_id, sizeof(dhcp_offer_server_id));
	ptr = bootp_add_option(ptr, (uint8_t *)&server_ip_addr, sizeof(server_ip_addr));

	ptr = bootp_add_option(ptr, (uint8_t *)dhcp_offer_lease_time, sizeof(dhcp_offer_lease_time));

	ptr = bootp_add_option(ptr, (uint8_t *)dhcp_offer_netmask, sizeof(dhcp_offer_netmask));
#if 0
	ptr = bootp_add_option(ptr, (uint8_t *)dhcp_offer_router, sizeof(dhcp_offer_router));
	ptr = bootp_add_option(ptr, (uint8_t *)&server_ip_addr, sizeof(server_ip_addr));
#endif
	*ptr = 0xFF; /* OPTION END */

	return 0;
}

int bootp_build_request(struct bootphdr *bph, uint8_t opcode,
		uint8_t hw_type, uint8_t hw_len, uint8_t *hw_addr) {
	if ((bph == NULL) || (hw_addr == NULL)) {
		return -EINVAL;
	}

	memset(bph, 0, sizeof(struct bootphdr) - sizeof(bph->vend));

	bph->op = opcode;
	bph->htype = hw_type;
	bph->hlen = hw_len;
	memcpy(&bph->chaddr[0], hw_addr, hw_len);

	memcpy(&bph->vend[0], &dhcp_magic_cookie[0], sizeof(dhcp_magic_cookie));
	/* setup options */
	memcpy(&bph->vend[sizeof(dhcp_magic_cookie)],
			dhcp_discovery_options, sizeof(dhcp_discovery_options));

	return 0;
}

static int bootp_valid(struct bootphdr *bph, uint8_t opcode) {
	if (bph == NULL) {
		return -EINVAL;
	}

	if ((bph->op != opcode) || (0 != memcmp(&bph->vend[0],
				&dhcp_magic_cookie[0], sizeof dhcp_magic_cookie))) {
			return -EINVAL;
	}

	return 0;
}

int bootp_find_option(struct bootphdr *bph, struct dhcp_option *opt) {
	uint8_t *tag;
	uint8_t *tag_len;

	tag = &bph->vend[sizeof(dhcp_magic_cookie)];
	while (tag < &bph->vend[sizeof(bph->vend)]) {
		switch(*tag) {
		default:
			if (*tag == opt->tag) {

				opt->len = tag[1];
				opt->value = &tag[2];

				return 1;
			}
			tag++;
			tag_len = tag;
			tag += (*tag_len + 1);
			break;
		case TAG_PAD:
			tag++;
			break;
		case TAG_END:
			return 0;
		}
	}

	return 0;
}

uint8_t *bootp_get_opt(struct bootphdr *bph, uint8_t opt_type) {
	int ret;
	uint8_t *tag;
	uint8_t *tag_len;

	ret = bootp_valid(bph, BOOTPREPLY);
	if (ret != 0) {
		return err2ptr(-ret);
	}

	tag = &bph->vend[sizeof(dhcp_magic_cookie)];
	while (tag < &bph->vend[sizeof(bph->vend)]) {
		switch(*tag) {
		default:
			if (*tag == opt_type) {
				return tag + 2;
			}
			tag++;
			tag_len = tag;
			tag += (*tag_len + 1);
			break;
		case TAG_PAD:
			tag++;
			break;
		case TAG_END:
			return NULL;
		}
	}

	return NULL;
}

int bootp_get_ip(struct bootphdr *bph, in_addr_t *ip){
	int ret;

	ret = bootp_valid(bph, BOOTPREPLY);
	if (ret != 0) {
		return ret;
	}
	*ip = bph->yiaddr;

	return 0;
}

int bootp_get_nameserver(struct bootphdr *bph, in_addr_t *ip) {
	uint8_t *ret;

	ret = bootp_get_opt(bph, TAG_NAME_SERVER);
	if (0 >= ret) {
		if (ret == 0) {
			return -ENOENT;
		} else {
			return (uintptr_t) ret;
		}
	}
	*ip = *(in_addr_t *)ret;

	return 0;
}

int bootp_get_gateway(struct bootphdr *bph, in_addr_t *ip) {
	uint8_t *ret;

	ret = bootp_get_opt(bph, TAG_GATEWAY);
	if (0 >= ret) {
		if (ret == 0) {
			return -ENOENT;
		} else {
			return (uintptr_t) ret;
		}
	}
	*ip = *(in_addr_t *)ret;

	return 0;
}

int bootp_get_mask(struct bootphdr *bph, in_addr_t *ip) {
	uint8_t *ret;

	ret = bootp_get_opt(bph, TAG_SUBNET_MASK);
	if (0 >= ret) {
		if (ret == 0) {
			return -ENOENT;
		} else {
			return (uintptr_t) ret;
		}
	}
	*ip = *(in_addr_t *)ret;

	return 0;
}
