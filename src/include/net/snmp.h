/**
 * @file
 * @brief SNMP
 *
 * @date 8.11.2012
 * @author Alexander Kalmuk
 */

#ifndef NET_SNMP_H_
#define NET_SNMP_H_

#include <types.h>
#include <util/dlist.h>

enum pdu_type {
	/* simple types */
	PDU_INTEGER      = 0x02,
	PDU_STRING       = 0x04,
	PDU_NULL         = 0x05,
	PDU_OID          = 0x06,
	/* complex types */
	PDU_SEQUENCE     = 0x30,
	PDU_GET_REQUEST  = 0xA0,
	PDU_GET_RESPONSE = 0xA2,
	PDU_SET_REQUEST  = 0xA3
};

typedef struct obj_data {
	enum pdu_type type;
	void *data;
	__u8 datalen;
} *obj_data_t;

typedef struct mib_obj {
	const char *name;
	int id;
	struct dlist_head parent_link;
	struct dlist_head children;
	obj_data_t data;
} *mib_obj_t;

extern struct mib_obj mib_root;

struct snmp_desc {
	__u32 version;
	char *security;
	__u32 error;
	__u32 error_index;
	enum pdu_type pdu_type;
	struct dlist_head varbind_list;
};

struct varbind {
	struct dlist_head link;
	__u8 oid_len;
	char *oid;
	obj_data_t data;
};

/* Functions for manipulating with MIB */
extern mib_obj_t mib_obj_alloc(void);
/*extern void mib_obj_free(void);*/
extern int mib_obj_link(mib_obj_t obj, mib_obj_t parent);
/* Unlink subtree with obj as root. */
/*extern void mib_obj_unlink(mib_obj_t obj);*/
extern mib_obj_t mib_obj_getbyoid(const char *oid);
/*extern mib_obj_t mib_obj_getbyname(const char *name);*/

/* Functions for manipulating with SNMP */
extern void snmp_build(struct snmp_desc *snmp_desc, const char *snmp_packet);
/* Reverse operation for snmp_op_build */
extern void snmp_parse(struct snmp_desc *snmp_desc, const char *snmp_packet);

#endif /* NET_SNMP_H_ */
