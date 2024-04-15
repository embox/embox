/**
 * @file
 * @brief MIB interface
 *
 * @date 14.11.2012
 * @author Alexander Kalmuk
 */

#ifndef NET_LIB_SNMP_MIB_H_
#define NET_LIB_SNMP_MIB_H_

#include <stdint.h>
#include <lib/libds/dlist.h>
#include <lib/libds/array.h>

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
	uint8_t datalen;
} *obj_data_t;

extern struct mib_obj mib_root;

typedef struct mib_obj {
	const char *name;
	uint8_t id;
	struct dlist_head parent_link;
	struct dlist_head children;
	obj_data_t data;
} *mib_obj_t;

typedef void (*mib_register_func)(void);

/* Functions for manipulating with MIB */
extern mib_obj_t mib_obj_alloc(void);
/*extern void mib_obj_free(void);*/
extern int mib_obj_link(mib_obj_t obj, mib_obj_t parent);
extern mib_obj_t mib_obj_addbyoid(const char *oid, unsigned char len);
/* Unlink subtree with obj as root. */
/*extern void mib_obj_unlink(mib_obj_t obj);*/
extern mib_obj_t mib_obj_getbyoid(const char *oid, unsigned char len);
/* Initialize all mibs */
extern int mib_init_all(void);
/*extern mib_obj_t mib_obj_getbyname(const char *name);*/

#define MIB_OBJECT_REGISTER(init) \
	ARRAY_SPREAD_DECLARE(const mib_register_func, \
			__mib_register); \
    ARRAY_SPREAD_ADD(__mib_register, init)

#endif /* NET_LIB_SNMP_MIB_H_ */
