/**
 * @file
 * @brief
 *
 * @date 22.08.12
 * @author Ilia Vaprol
 */

#include <net/netdb.h>
#include <net/dns.h>
#include <net/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int dns_header(struct dnshdr *dnsh) {
	/* Clear header's fields */
	memset(dnsh, 0, sizeof *dnsh);

	/* Setup new values */
	dnsh->id = 0x1234; /* TODO */
	dnsh->qr = DNS_MSG_TYPE_QUERY;
	dnsh->opcode = DNS_OPER_CODE_QUERY;
	dnsh->rd = 1;
	dnsh->qdcount = 1;

	return 0;
}

static int dns_format_name(const char *name, char *buff, size_t buff_sz, size_t *out_formatted) {
	return -ENOSYS;
}

static int dns_q_format(struct dns_q *query, char *buff,
		size_t buff_sz, size_t *out_formatted) {
	int ret;
	size_t bytes_left, field_sz;

	bytes_left = buff_sz;

	ret = dns_format_name(&query->qname[0], buff, buff_sz, &field_sz);
	if (ret != 0) {
		return ret;
	}
	bytes_left -= field_sz;
	buff += field_sz;

	field_sz = sizeof query->qtype;
	if (field_sz > bytes_left) {
		return -ENOMEM;
	}
	*(__be16 *)buff = htons(query->qtype);
	bytes_left -= field_sz;
	buff += field_sz;

	field_sz = sizeof query->qclass;
	if (field_sz > bytes_left) {
		return -ENOMEM;
	}
	*(__be16 *)buff = htons(query->qclass);
	bytes_left -= field_sz;
	buff += field_sz;

	return 0;
}

static int dns_query_execute(struct dns_q *query,
		struct dns_rr **out_result, size_t *out_result_amount) {
	int ret;
	union {
		struct dnshdr dnsh;
		char raw[DNS_MAX_MESSAGE_SZ];
	} msg;
	size_t msg_size, formatted;

	ret = dns_header(&msg.dnsh);
	if (ret != 0) {
		return ret;
	}
	msg_size = sizeof msg.dnsh;

	ret = dns_q_format(query, &msg.raw[msg_size], sizeof msg - msg_size, &formatted);
	if (ret != 0) {
		return ret;
	}
	msg_size += formatted;

	/* TODO send msg */

	return 0;
}

int dns_query(const char *qname, enum dns_type qtype, enum dns_class qclass,
		struct dns_rr **out_result, size_t *out_amount) {
	struct dns_q query;
	size_t qname_sz;

	qname_sz = strlen(qname) + 1;
	if (qname_sz > sizeof query.qname) {
		return -EINVAL;
	}

	memcpy(&query.qname[0], qname, qname_sz);
	query.qtype = qtype;
	query.qclass = qclass;

	return dns_query_execute(&query, out_result, out_amount);
}

