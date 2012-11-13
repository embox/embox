/**
 * @file
 * @brief SNMP
 *
 * @date 9.11.2012
 * @author Alexander Kalmuk
 */

#include <net/snmp.h>
#include <errno.h>
#include <string.h>

static __u32 __id;

static __u32 snmp_len(struct snmp_desc *snmp_desc);
static __u32 pdu_len(struct snmp_desc *snmp_desc);
static __u32 data_len(struct snmp_desc *snmp_desc);
static void fill_snmp(char *dst, char *src, enum pdu_type type, __u32 len);
static void extract_snmp(char *dst, char *src);

void snmp_build(struct snmp_desc *snmp_desc, const char *snmp_packet) {
	__u32 *cur = (__u32 *) snmp_packet;
	struct varbind *var, *nxt;

	__id++;

	*(cur++) = PDU_SEQUENCE;
	*(cur++) = snmp_len(snmp_desc);
	fill_snmp((char *)cur, (char *)&snmp_desc->version, PDU_INTEGER, 4);
	fill_snmp((char *)cur, (char *)&snmp_desc->security, PDU_STRING, strlen(snmp_desc->security) - 1);

	/* Fill PDU */
	*(cur++) = snmp_desc->pdu_type;
	*(cur++) = pdu_len(snmp_desc);
	fill_snmp((char *)cur, (char *)&__id, PDU_INTEGER, 4);
	fill_snmp((char *)cur, (char *)&snmp_desc->error, PDU_INTEGER, 4);
	fill_snmp((char *)cur, (char *)&snmp_desc->error_index, PDU_INTEGER, 4);

	*(cur++) = PDU_SEQUENCE;
	*(cur++) = data_len(snmp_desc);
	dlist_foreach_entry(var, nxt, &snmp_desc->varbind_list, link) {
		fill_snmp((char *)cur, var->oid, PDU_OID, var->oid_len);
		if (var->data) {
			fill_snmp((char *)cur, var->data->data, var->data->type, var->data->datalen);
		}
	}
}

void snmp_parse(struct snmp_desc *snmp_desc, const char *snmp_recv) {
	char *cur = (char*)snmp_recv;
	__u32 len;

	cur += 8; /* PDU_SEQUENCE, length */
	extract_snmp((char*)&snmp_desc->version, cur);
	extract_snmp((char*)&snmp_desc->security, cur);

	cur += 8; /* pdu_type, length*/
	extract_snmp(NULL, cur); /* id */
	extract_snmp((char*)&snmp_desc->error, cur);
	extract_snmp((char*)&snmp_desc->error_index, cur);

	cur += 4; /* PDU_SEQUENCE */
	len = *(cur++); /* PDU_SEQUENCE length */
	while (len > 0) {

	}
}

static void fill_snmp(char *dst, char *src, enum pdu_type type, __u32 len) {
	__u32 *str = (__u32 *) dst;

	*(str++) = type;
	*(str++) = len;
	memcpy(dst, src, len);
	dst += sizeof type + sizeof len + len;
}

static void extract_snmp(char *dst, char *src) {
	__u32 len;
	src += 4;
	len = *(int*)src;
	src += 4;
	memcpy(dst, src, len);
	src += len;
}

static __u32 snmp_len(struct snmp_desc *snmp_desc) {
	return (pdu_len(snmp_desc) + strlen(snmp_desc->security) - 1 +
			sizeof(snmp_desc->version));
}

static __u32 pdu_len(struct snmp_desc *snmp_desc) {
	return (data_len(snmp_desc) + sizeof(snmp_desc->error)
			+ sizeof(snmp_desc->error_index) + sizeof(__id));
}

static __u32 data_len(struct snmp_desc *snmp_desc) {
	struct varbind *var, *nxt;
	__u32 len = 0;

	dlist_foreach_entry(var, nxt, &snmp_desc->varbind_list, link) {
		len += var->oid_len;
		if (var->data) {
			len += var->data->datalen;
		}
	}

	return len;
}
