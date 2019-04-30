/**
 * @file
 * @brief SNMPv1
 *
 * @date 9.11.2012
 * @author Alexander Kalmuk
 */
#include <stdint.h>

#include <net/lib/snmp.h>
#include <errno.h>
#include <string.h>
#include <mem/objalloc.h>

#define __HDR_LEN 2 /* length(type) + length(len): |type|len|data... */

static uint32_t __id;

static uint8_t snmp_len(struct snmp_desc *snmp_desc);
static uint8_t pdu_len(struct snmp_desc *snmp_desc);
static uint8_t data_len(struct snmp_desc *snmp_desc);
static uint8_t var_len(struct varbind *var);
static int is_snmp_response(struct snmp_desc *snmp_desc);
static void fill_snmp(char **dst, char *src, enum pdu_type type, uint32_t len);
static uint8_t extract_snmp(char *dst, unsigned char **src);
static int snmp_alloc_var(struct varbind *var, char **userbuf, size_t *bufsize);

uint32_t snmp_build(struct snmp_desc *snmp_desc, const char *snmp_packet) {
	char *cur = (char*)snmp_packet;
	struct varbind *var;
	uint32_t packet_len = 0;

	/* Fill SNMP: inclusion: snmp<-pdu<-data */
	*(cur++) = (char)PDU_SEQUENCE;
	packet_len = *(cur++) = snmp_len(snmp_desc);
	packet_len += __HDR_LEN;
	fill_snmp(&cur, (char *)&snmp_desc->version, PDU_INTEGER, 1);
	fill_snmp(&cur, snmp_desc->security, PDU_STRING, strlen(snmp_desc->security));

	/* Fill PDU */
	*(cur++) = snmp_desc->pdu_type;
	*(cur++) = pdu_len(snmp_desc);

	/* Set id */
	if (!is_snmp_response(snmp_desc)) {
		__id++;
		fill_snmp(&cur, (char *)&__id, PDU_INTEGER, 4);
	} else {
		fill_snmp(&cur, (char *)&snmp_desc->id, PDU_INTEGER, 4);
	}

	fill_snmp(&cur, (char *)&snmp_desc->error, PDU_INTEGER, 1);
	fill_snmp(&cur, (char *)&snmp_desc->error_index, PDU_INTEGER, 1);

	*(cur++) = PDU_SEQUENCE;
	*(cur++) = data_len(snmp_desc);

	/* Fill data */
	dlist_foreach_entry(var, &snmp_desc->varbind_list, link) {
		*(cur++) = PDU_SEQUENCE;
		*(cur++) = var_len(var);

		fill_snmp(&cur, var->oid, PDU_OID, var->oid_len);
		if (var->data) {
			fill_snmp(&cur, var->data->data, var->data->type, var->data->datalen);
		} else {
			fill_snmp(&cur, NULL, PDU_NULL, 0);
		}
	}

	return packet_len;
}

int snmp_parse(struct snmp_desc *snmp_desc, const char *snmp_recv, char *userbuf, size_t bufsize) {
	uint32_t len;
	unsigned char *cur = (unsigned char*)snmp_recv;

	/* Extract SNMP: inclusion: snmp<-pdu<-data */
	cur += __HDR_LEN; /* snmp main header*/
	extract_snmp((char*)&snmp_desc->version, &cur);

	/* extract security string */
	len = extract_snmp(userbuf, &cur);
	snmp_desc->security = userbuf;
	userbuf += len;
	*(++userbuf) = 0; /* terminate string, because in snmp packet all strings without terminated character */
	bufsize -= len + 1;

	/* Extract PDU*/
	snmp_desc->pdu_type = *cur; /* pdu header */
	cur += 2;
	extract_snmp((char*)&snmp_desc->id, &cur);
	extract_snmp((char*)&snmp_desc->error, &cur);
	extract_snmp((char*)&snmp_desc->error_index, &cur);

	dlist_init(&snmp_desc->varbind_list);

	/* Extract data */
	cur++;
	len = *(cur++); /* all data length */

	/* Extract data */
	while (len > 0) {
		struct varbind *var = (struct varbind *)userbuf;
		/* allocate variable and data in user's pool */
		if (snmp_alloc_var(var, &userbuf, &bufsize) == -1) {
			return -1;
		}

		dlist_head_init(&var->link);
		dlist_add_prev(&var->link, &snmp_desc->varbind_list);

		len -= *(++cur) + __HDR_LEN; /* actual data + header */
		cur++;

		/* oid */
		var->oid = (char *)(cur + __HDR_LEN);
		var->oid_len = extract_snmp(var->oid, &cur);

		/* value */
		var->data->type = *cur;
		var->data->data = (char *)(cur + __HDR_LEN);
		var->data->datalen = extract_snmp(var->data->data, &cur);
	}

	return 0;
}

static void fill_snmp(char **pdst, char *src, enum pdu_type type, uint32_t len) {
	char *dst = *pdst;

	*pdst += len + __HDR_LEN;

	*(dst++) = type;
	*(dst++) = len;

	memcpy(dst, src, len);
}

static uint8_t extract_snmp(char *dst, unsigned char **psrc) {
	unsigned char *src = *psrc;
	uint8_t len;

	len = *(++src);
	memcpy(dst, ++src, len);
	*psrc += len + __HDR_LEN;

	return len;
}

static int is_snmp_response(struct snmp_desc *snmp_desc) {
	return (snmp_desc->pdu_type == PDU_GET_RESPONSE);
}

static int snmp_alloc_var(struct varbind *var, char **userbuf, size_t *bufsize) {
	size_t data_len = sizeof(struct varbind) + sizeof(struct obj_data);

	if (*bufsize < data_len) {
		return -1;
	}

	var = (struct varbind *)*userbuf;
	*userbuf += sizeof(struct varbind);
	var->data = (struct obj_data *)*userbuf;
	*userbuf += sizeof(struct obj_data);

	*bufsize -= data_len;

	return 0;
}

static uint8_t snmp_len(struct snmp_desc *snmp_desc) {
	/* +2 bytes for every field (i.e. 2*2) + 2 for pdu header */
	return (pdu_len(snmp_desc) + strlen(snmp_desc->security) +
			sizeof(snmp_desc->version) + 2 * __HDR_LEN + __HDR_LEN);
}

static uint8_t pdu_len(struct snmp_desc *snmp_desc) {
	/* +2 for data header +data_len +2 bytes for every fields (i.e. +2*3) */
	return (data_len(snmp_desc) + sizeof(snmp_desc->error)
			+ sizeof(snmp_desc->error_index) + sizeof(__id) + __HDR_LEN * 3 + __HDR_LEN);
}

static uint8_t data_len(struct snmp_desc *snmp_desc) {
	struct varbind *var;
	uint8_t len = 0;

	dlist_foreach_entry(var, &snmp_desc->varbind_list, link) {
		len += var_len(var) + __HDR_LEN; /* actual data and header */
	}

	return len;
}

static uint8_t var_len(struct varbind *var) {
	uint8_t len = 0;

	len += var->oid_len;
	if (var->data) {
		len += var->data->datalen;
	}
	len += __HDR_LEN * 2; /* oid and value types and lengths - 2 + 2 bytes */

	return len;
}
