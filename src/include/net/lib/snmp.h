/**
 * @file
 * @brief SNMPv1
 *
 * @date 8.11.2012
 * @author Alexander Kalmuk
 */

#ifndef NET_LIB_SNMP_H_
#define NET_LIB_SNMP_H_

#include <stdint.h>
#include <lib/libds/dlist.h>
#include <net/lib/snmp_mib.h>

struct snmp_desc {
	uint8_t version;
	char *security;
	uint32_t id;
	uint8_t error;
	uint8_t error_index;
	enum pdu_type pdu_type;
	struct dlist_head varbind_list;
};

struct varbind {
	struct dlist_head link;
	uint8_t oid_len;
	char *oid;
	obj_data_t data;
};

/* Functions for manipulating with SNMP */
extern uint32_t snmp_build(struct snmp_desc *snmp_desc, const char *snmp_packet);
/* Reverse operation for snmp_op_build */
extern int snmp_parse(struct snmp_desc *snmp_desc, const char *snmp_recv, char *userbuf, size_t bufsize);

#endif /* NET_LIB_SNMP_H_ */
