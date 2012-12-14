/**
 * @file
 * @brief SNMPv1
 *
 * @date 8.11.2012
 * @author Alexander Kalmuk
 */

#ifndef NET_SNMP_H_
#define NET_SNMP_H_

#include <types.h>
#include <util/dlist.h>
#include <net/mib.h>

struct snmp_desc {
	__u8 version;
	char *security;
	__u32 id;
	__u8 error;
	__u8 error_index;
	enum pdu_type pdu_type;
	struct dlist_head varbind_list;
};

struct varbind {
	struct dlist_head link;
	__u8 oid_len;
	char *oid;
	obj_data_t data;
};

/* Functions for manipulating with SNMP */
extern __u32 snmp_build(struct snmp_desc *snmp_desc, const char *snmp_packet);
/* Reverse operation for snmp_op_build */
extern int snmp_parse(struct snmp_desc *snmp_desc, const char *snmp_recv, char *userbuf, size_t bufsize);

#endif /* NET_SNMP_H_ */
