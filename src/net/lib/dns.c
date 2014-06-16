/**
 * @file
 * @brief
 *
 * @date 22.08.12
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <netdb.h>
#include <arpa/inet.h>

#include <net/lib/dns.h>

#include <net/l3/ipv4/ip.h>
#include <sys/socket.h>
#include <kernel/printk.h>
#include <framework/mod/options.h>

/**
 * DNS nameservers
 */
#define MODOPS_NAMESERVER OPTION_STRING_GET(nameserver)

/**
 * DNS query timeout
 */
#define MODOPS_DNS_QUERY_TIMEOUT OPTION_GET(NUMBER, dns_query_timeout)

union dns_msg {
	char raw[DNS_MAX_MESSAGE_SZ];
	struct {
		struct dnshdr hdr;
		char data[1];
	} msg;
};

static int name_to_label(const char *name, char *buff,
		size_t buff_sz, size_t *out_bytes) {
	char *dot;
	size_t bytes_left, field_sz;

	bytes_left = buff_sz;

	do {
		dot = strchr(name, '.');
		field_sz = (dot != NULL ? dot - name : strlen(name)) * sizeof(char);
		if (field_sz > DNS_MAX_LABEL_SZ) {
			return -EINVAL;
		}

		if (sizeof(uint8_t) > bytes_left) {
			return -ENOMEM;
		}
		*(uint8_t *)buff = (uint8_t)field_sz;
		bytes_left -= sizeof(uint8_t);
		buff += sizeof(uint8_t);

		if (field_sz > bytes_left) {
			return -ENOMEM;
		}
		memcpy(buff, name, field_sz);
		bytes_left -= field_sz;
		buff += field_sz;

		name = dot + 1;

	} while (dot != NULL);

	if (sizeof(uint8_t) > bytes_left) {
		return -ENOMEM;
	}
	*(uint8_t *)buff = 0;
	bytes_left -= sizeof(uint8_t);
	buff += sizeof(uint8_t);

	*out_bytes = buff_sz - bytes_left;

	return 0;
}

static int label_to_name(const char *label, const char *buff, size_t buff_sz,
		size_t max_name_sz, char *out_name, size_t *out_field_sz) {
	size_t bytes_left, field_sz;
	uint8_t label_sz;
	uint16_t offset;
	char one_line;
	const char *buff_end;

	one_line = 1;
	bytes_left = max_name_sz;
	field_sz = 0;
	buff_end = buff + buff_sz;

	while ((label_sz = *(uint8_t *)label++) != 0) {
		/* It's a pointer to other label? */
		if ((label_sz & DNS_LABEL_MASK) == DNS_LABEL_MASK) {
			if (one_line) {
				field_sz += 2 * sizeof(char); /* plus size of pointer */
				one_line = 0;
			}
			offset = ((label_sz & ~DNS_LABEL_MASK) << 8) + *(uint8_t *)label;
			label = buff + offset; /* get new address of label */
			if (label >= buff_end) { /* check boundary */
				return -ENOMEM;
			}
			continue;
		}

		if ((label_sz > bytes_left) || (label + label_sz > buff_end)) {
			return -ENOMEM;
		}
		memcpy(out_name, label, label_sz * sizeof(char));
		bytes_left -= label_sz * sizeof(char);
		label += label_sz;
		out_name += label_sz;

		/* How much bytes will be skipped after parsing of lable? */
		if (one_line) {
			field_sz += (label_sz + 1) * sizeof(char);
		}

		if (sizeof(char) > bytes_left) {
			return -ENOMEM;
		}
		*out_name++ = '.';
		bytes_left -= sizeof(char);
	}

	/**
	 * If we didn't have loop's iteration, we must check memory for '\0'
	 * otherwise we replace '.' to it
	 */
	if (bytes_left != max_name_sz) {
		out_name--;
	} else if ((sizeof(char) > bytes_left) || (label + 1 > buff_end)) {
		return -ENOMEM;
	} else {
		bytes_left -= sizeof(char);
	}
	*out_name++ = 0;

	if (out_field_sz != NULL) {
		*out_field_sz = field_sz
				+ (one_line /* if it's haven't labels */
						? sizeof(char) /* plus one byte at the end
										  of the loop */
						: 0);
	}

	return 0;
}

static int dns_q_format(struct dns_q *query, char *buff,
		size_t buff_sz, size_t *out_bytes) {
	int ret;
	size_t bytes_left, field_sz;
	uint16_t field_val;

	bytes_left = buff_sz;

	ret = name_to_label(&query->qname[0], buff, buff_sz, &field_sz);
	if (ret != 0) {
		return ret;
	}
	bytes_left -= field_sz;
	buff += field_sz;

	field_sz = sizeof query->qtype;
	if (field_sz > bytes_left) {
		return -ENOMEM;
	}
	field_val = htons(query->qtype);
	memcpy(buff, &field_val, sizeof field_sz);
	bytes_left -= field_sz;
	buff += field_sz;

	field_sz = sizeof query->qclass;
	if (field_sz > bytes_left) {
		return -ENOMEM;
	}
	field_val = htons(query->qclass);
	memcpy(buff, &field_val, sizeof field_sz);
	bytes_left -= field_sz;
	buff += field_sz;

	*out_bytes = buff_sz - bytes_left;

	return 0;
}

static int dns_query_format(struct dns_q *query, union dns_msg *dm,
		size_t *out_dm_sz) {
	int ret;
	size_t data_sz;

	/* Setup header fields */
	memset(&dm->msg.hdr, 0, sizeof dm->msg.hdr);
	dm->msg.hdr.id = 0x1234; /* TODO */
	dm->msg.hdr.qr = DNS_MSG_TYPE_QUERY;
	dm->msg.hdr.opcode = DNS_OPER_CODE_QUERY;
	dm->msg.hdr.rd = 1;
	dm->msg.hdr.qdcount = htons(1);

	/* Format data section */
	ret = dns_q_format(query, &dm->msg.data[0],
			sizeof *dm - sizeof dm->msg.hdr, &data_sz);
	if (ret != 0) {
		return ret;
	}

	/* Save the total size fo message */
	*out_dm_sz = sizeof dm->msg.hdr + data_sz;

	return 0;
}

static int dns_query_execute(union dns_msg *req, size_t req_sz,
		union dns_msg *rep, size_t *out_rep_sz) {
	static const struct timeval timeout = {
		.tv_sec = MODOPS_DNS_QUERY_TIMEOUT / MSEC_PER_SEC,
		.tv_usec = (MODOPS_DNS_QUERY_TIMEOUT % MSEC_PER_SEC) * USEC_PER_MSEC,
	};
	int sock;
	ssize_t bytes;
	struct sockaddr_in nameserver_addr;

	/* Setup dns_host structure */
	memset(&nameserver_addr, 0, sizeof nameserver_addr);
	nameserver_addr.sin_family = AF_INET;
	nameserver_addr.sin_port = htons(DNS_PORT_NUMBER);
	if (!inet_aton(dns_get_nameserver(), &nameserver_addr.sin_addr)) {
		return -EINVAL;
	}

	/* Create socket */
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == -1) {
		return -errno;
	}

	if (-1 == connect(sock, (struct sockaddr *)&nameserver_addr,
				sizeof nameserver_addr)) {
		close(sock);
		return -errno;
	}

	if (-1 == setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
				&timeout, sizeof timeout)) {
		close(sock);
		return -errno;
	}

	while (1) {
		/* Send request */
		bytes = send(sock, &req->raw[0], req_sz, 0);
		if (bytes != req_sz) {
			close(sock);
			return -errno;
		}

		do {
			/* Receive reply */
			bytes = recv(sock, &rep->raw[0], sizeof *rep, 0);
			if (bytes == -1) {
				close(sock);
				return -errno;
			}
		} while (bytes < sizeof(struct dnshdr)); /* bad size, try again */

		/* Is it my? */
		if (req->msg.hdr.id != rep->msg.hdr.id) {
			continue;
		}

		/* all ok, done */
		break;
	}

	/* Close our socket */
	close(sock);

	/* Setup result size */
	*out_rep_sz = (size_t)bytes;

	return 0;
}

static int dns_q_parse(struct dns_q *q, const char *data,
		const char *buff, size_t buff_sz, size_t *out_field_sz) {
	int ret;
	size_t field_sz;
	uint16_t field_val;
	const char *curr, *end;

	curr = data;
	end = buff + buff_sz;

	/* parse name */
	ret = label_to_name(curr, buff, buff_sz,
			sizeof q->qname, &q->qname[0], &field_sz);
	if (ret != 0) {
		return ret;
	}
	curr += field_sz;

	/* parse type */
	field_sz = sizeof(uint16_t);
	if (curr + field_sz > end) {
		return -EINVAL;
	}
	memcpy(&field_val, curr, field_sz);
	q->qtype = ntohs(field_val);
	curr += field_sz;

	/* parse class */
	field_sz = sizeof(uint16_t);
	if (curr + field_sz > end) {
		return -EINVAL;
	}
	memcpy(&field_val, curr, field_sz);
	q->qclass = ntohs(field_val);
	curr += field_sz;

	*out_field_sz = curr - data;

	return 0;
}

static int dns_rr_a_parse(struct dns_rr *rr, const char *data, size_t field_sz,
		const char *buff, size_t buff_sz) {
	if (field_sz != sizeof rr->rdata.a.address) {
		return -EINVAL;
	}
	memcpy(&rr->rdata.a.address[0], data, sizeof rr->rdata.a.address);
	return 0;
}

static int dns_rr_ns_parse(struct dns_rr *rr, const char *data, size_t field_sz,
		const char *buff, size_t buff_sz) {
	return label_to_name(data, buff, buff_sz, sizeof rr->rdata.ns.nsdname,
			&rr->rdata.ns.nsdname[0], NULL);
}

static int dns_rr_cname_parse(struct dns_rr *rr, const char *data, size_t field_sz,
		const char *buff, size_t buff_sz) {
	return label_to_name(data, buff, buff_sz, sizeof rr->rdata.cname.cname,
			&rr->rdata.cname.cname[0], NULL);
}

static int dns_rr_ptr_parse(struct dns_rr *rr, const char *data, size_t field_sz,
		const char *buff, size_t buff_sz) {
	return label_to_name(data, buff, buff_sz, sizeof rr->rdata.ptr.ptrdname,
			&rr->rdata.ptr.ptrdname[0], NULL);
}

static int dns_rr_aaaa_parse(struct dns_rr *rr, const char *data, size_t field_sz,
		const char *buff, size_t buff_sz) {
	if (field_sz != sizeof rr->rdata.aaaa.address) {
		return -EINVAL;
	}
	memcpy(&rr->rdata.aaaa.address[0], data, sizeof rr->rdata.aaaa.address);
	return 0;
}

static int dns_rr_parse(struct dns_rr *rr, const char *data,
		const char *buff, size_t buff_sz, size_t *out_field_sz) {
	int ret;
	size_t field_sz;
	uint16_t field_val16;
	uint32_t field_val32;
	const char *curr, *end;

	curr = data;
	end = buff + buff_sz;

	/* parse name */
	ret = label_to_name(curr, buff, buff_sz,
			sizeof rr->rname, &rr->rname[0], &field_sz);
	if (ret != 0) {
		return ret;
	}
	curr += field_sz;

	/* parse type */
	field_sz = sizeof(uint16_t);
	if (curr + field_sz > end) {
		return -EINVAL;
	}
	memcpy(&field_val16, curr, field_sz);
	rr->rtype = ntohs(field_val16);
	curr += field_sz;

	/* parse class */
	field_sz = sizeof(uint16_t);
	if (curr + field_sz > end) {
		return -EINVAL;
	}
	memcpy(&field_val16, curr, field_sz);
	rr->rclass = ntohs(field_val16);
	curr += field_sz;

	/* parse ttl */
	field_sz = sizeof(uint32_t);
	if (curr + field_sz > end) {
		return -EINVAL;
	}
	memcpy(&field_val32, curr, field_sz);
	rr->rttl = ntohl(field_val32);
	curr += field_sz;

	/* parse data length */
	field_sz = sizeof(uint16_t);
	if (curr + field_sz > end) {
		return -EINVAL;
	}
	memcpy(&field_val16, curr, field_sz);
	rr->rdlength = ntohs(field_val16);
	curr += field_sz;

	/* parse data */
	field_sz = rr->rdlength;
	if (curr + field_sz > end) {
		return -EINVAL;
	}
	switch (rr->rtype) { /* TODO use table of methods instead */
	default:
		ret = -ENOSYS;
		printk("dns_rr_parse: error: can't parse type %d\n", rr->rtype);
		break;
	case DNS_RR_TYPE_A:
		ret = dns_rr_a_parse(rr, curr, field_sz, buff, buff_sz);
		break;
	case DNS_RR_TYPE_NS:
		ret = dns_rr_ns_parse(rr, curr, field_sz, buff, buff_sz);
		break;
	case DNS_RR_TYPE_CNAME:
		ret = dns_rr_cname_parse(rr, curr, field_sz, buff, buff_sz);
		break;
	case DNS_RR_TYPE_PTR:
		ret = dns_rr_ptr_parse(rr, curr, field_sz, buff, buff_sz);
		break;
	case DNS_RR_TYPE_AAAA:
		ret = dns_rr_aaaa_parse(rr, curr, field_sz, buff, buff_sz);
		break;
	}
	if (ret != 0) {
		return ret;
	}
	curr += field_sz;

	*out_field_sz = curr - data;

	return 0;
}

static int dns_result_parse(union dns_msg *dm, size_t dm_sz,
		struct dns_result *out_result) {
	int ret;
	const char *curr;
	struct dns_q *qs;
	struct dns_rr *rrs;
	size_t i, amount, section_sz;

	curr = &dm->msg.data[0];
	memset(out_result, 0, sizeof *out_result);

	/* Parse Header section */
	if (dm->msg.hdr.qr != DNS_MSG_TYPE_REPLY) {
		return -EINVAL;
	}

	if (dm->msg.hdr.rcode != DNS_RESP_CODE_OK) {
		printk("dns_result_parse: error: DNS result code is %d!\n", dm->msg.hdr.rcode);
		return -1;
	}

	/* Parse Question section */
	amount = htons(dm->msg.hdr.qdcount);
	if (amount != 0) {
		qs = malloc(amount * sizeof *qs);
		if (qs == NULL) {
			ret = -ENOMEM;
			goto error;
		}
	}
	else {
		qs = NULL;
	}

	out_result->qdcount = amount;
	out_result->qd = qs;

	for (i = 0; i < amount; ++i) {
		ret = dns_q_parse(qs++, curr, &dm->raw[0], dm_sz, &section_sz);
		if (ret != 0) {
			goto error;
		}
		curr += section_sz;
	}

	/* Parse Answer section */
	amount = htons(dm->msg.hdr.ancount);
	if (amount != 0) {
		rrs = malloc(amount * sizeof *rrs);
		if (rrs == NULL) {
			ret = -ENOMEM;
			goto error;
		}
	}
	else {
		rrs = NULL;
	}

	out_result->ancount = amount;
	out_result->an = rrs;

	for (i = 0; i < amount; ++i) {
		ret = dns_rr_parse(rrs++, curr, &dm->raw[0], dm_sz, &section_sz);
		if (ret != 0) {
			goto error;
		}
		curr += section_sz;
	}

	/* Parse Authority section */
	amount = htons(dm->msg.hdr.nscount);
	if (amount != 0) {
		rrs = malloc(amount * sizeof *rrs);
		if (rrs == NULL) {
			ret = -ENOMEM;
			goto error;
		}
	}
	else {
		rrs = NULL;
	}

	out_result->nscount = amount;
	out_result->ns = rrs;

	for (i = 0; i < amount; ++i) {
		ret = dns_rr_parse(rrs++, curr, &dm->raw[0], dm_sz, &section_sz);
		if (ret != 0) {
			goto error;
		}
		curr += section_sz;
	}

	/* Parse Additional section */
	amount = htons(dm->msg.hdr.arcount);
	if (amount != 0) {
		rrs = malloc(amount * sizeof *rrs);
		if (rrs == NULL) {
			ret = -ENOMEM;
			goto error;
		}
	}
	else {
		rrs = NULL;
	}

	out_result->arcount = amount;
	out_result->ar = rrs;

	for (i = 0; i < amount; ++i) {
		ret = dns_rr_parse(rrs++, curr, &dm->raw[0], dm_sz, &section_sz);
		if (ret != 0) {
			goto error;
		}
		curr += section_sz;
	}

	/* That's all */
	if (curr != &dm->raw[dm_sz]) {
		return -EINVAL;
	}

	/* All ok, done */
	return 0;

error:
	dns_result_free(out_result);
	return ret;
}

static int dns_execute(struct dns_q *query, struct dns_result *out_result) {
	int ret;
	union dns_msg msg_in, msg_out;
	size_t msg_in_sz = 0;
	size_t msg_out_sz;

	ret = dns_query_format(query, &msg_out, &msg_out_sz);
	if (ret != 0) {
		return ret;
	}

	ret = dns_query_execute(&msg_out, msg_out_sz, &msg_in, &msg_in_sz);
	if (ret != 0) {
		return ret;
	}

	ret = dns_result_parse(&msg_in, msg_in_sz, out_result);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

int dns_query(const char *qname, enum dns_type qtype, enum dns_class qclass,
		struct dns_result *out_result) {
	struct dns_q query;
	size_t qname_sz;

	qname_sz = strlen(qname) + 1;
	if (qname_sz > sizeof query.qname) {
		return -EINVAL;
	}

	memcpy(&query.qname[0], qname, qname_sz);
	query.qtype = qtype;
	query.qclass = qclass;

	return dns_execute(&query, out_result);
}

int dns_result_free(struct dns_result *result) {
	free(result->qd);
	free(result->an);
	free(result->ns);
	free(result->ar);
	return 0;
}

const char * dns_get_nameserver(void) {
	return MODOPS_NAMESERVER;
}
