/**
 * @file
 * @brief Network type pack defs
 *
 * @date 02.11.2011
 * @author Anton Kozlov
 */

struct sk_buff;

static inline void *pnet_pack_get_data(net_pack_t pack) {
	return pack->skbuff->data;
}

static inline int pnet_pack_get_len(net_pack_t pack) {
	return pack->skbuff->len;
}

struct net_packet {
	enum net_packet_dir dir;

	net_node_t node;

	struct sk_buff *skbuf;
};

