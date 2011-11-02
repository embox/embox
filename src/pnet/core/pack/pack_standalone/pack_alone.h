/**
 * @file
 * @brief Network type pack defs
 *
 * @date 02.11.2011
 * @author Anton Kozlov
 */

struct sk_buff;

struct net_packet {
	enum net_packet_dir dir;

	net_node_t node;

	void *data;
	int len;
};

static inline void *pnet_pack_get_data(struct net_packet *pack) {
	return pack->data;
}

static inline int pnet_pack_get_len(struct net_packet *pack) {
	return pack->len;
}

