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
#include <net/ip.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <net/socket.h>
#include <stdio.h>
#include <err.h>

#include <framework/mod/options.h>
#define MODOPS_NAMESERVER OPTION_STRING_GET(nameserver)

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
	uint8_t label_sz, offset;
	char one_line;
	const char *buff_end;

	one_line = 1;
	bytes_left = max_name_sz;
	field_sz = 0;
	buff_end = buff + buff_sz;

	while ((label_sz = *(uint8_t *)label++) != 0) {
		if (label_sz == 0xC0) { /* it's a pointer to other label */
			one_line = 0;
			offset = *(uint8_t *)label;
			label = buff + offset; /* get new address of label */
			if (label >= buff_end) { /* check boundary */
				return -ENOMEM;
			}
			continue;
		}

		if ((label_sz > bytes_left) || (label + label_sz >= buff_end)) {
			return -ENOMEM;
		}
		memcpy(out_name, label, label_sz * sizeof(char));
		bytes_left -= label_sz * sizeof(char);
		label += label_sz;
		out_name += label_sz;

		/* How much bytes will be skipped after parsing of lable ? */
		if (one_line) {
			field_sz += (label_sz + 1) * sizeof(char);
		}

		if (sizeof(char) > bytes_left) {
			return -ENOMEM;
		}
		*out_name++ = '.';
		bytes_left -= sizeof(char);
		label++;
		out_name++;
	}

	/**
	 * If we didn't have loop's iteration, we must check memory for '\0'
	 * otherwise we replace '.' to it
	 */
	if (bytes_left != max_name_sz) {
		out_name--;
	} else if ((sizeof(char) > bytes_left) || (label + 1 >= buff_end)) {
		return -ENOMEM;
	} else {
		bytes_left -= sizeof(char);
	}
	*out_name++ = 0;

	if (out_field_sz != NULL) {
		*out_field_sz = field_sz + sizeof(char); /* plus one byte at the end of the loop */
	}

	return 0;
}

static int dns_q_format(struct dns_q *query, char *buff,
		size_t buff_sz, size_t *out_bytes) {
	int ret;
	size_t bytes_left, field_sz;

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
	int sock;
	ssize_t bytes;
	struct sockaddr_in nameserver_addr;
	socklen_t nameserver_addr_sz;

	/* Setup dns_host structure */
	memset(&nameserver_addr, 0, sizeof nameserver_addr);
	nameserver_addr.sin_family = AF_INET;
	nameserver_addr.sin_port = htons(DNS_PORT_NUMBER);
	if (!inet_aton(MODOPS_NAMESERVER, &nameserver_addr.sin_addr)) {
		return -EINVAL;
	}
	nameserver_addr_sz = sizeof nameserver_addr;

	/* Create socket */
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		return sock;
	}

	do {
		/* Send request */
		bytes = sendto(sock, &req->raw[0], req_sz, 0,
				(struct sockaddr *)&nameserver_addr, nameserver_addr_sz);
		if (bytes != req_sz) {
			close(sock);
			return errno ? -errno : -1; /* if errno equal to zero O_o */
		}

		/* Receive reply */
		bytes = recvfrom(sock, &rep->raw[0], sizeof *rep, 0,
				(struct sockaddr *)&nameserver_addr, &nameserver_addr_sz);
		if (bytes < 0) {
			close(sock);
			return errno ? -errno : -1; /* if errno equal to zero O_o */
		}
		else if (bytes < sizeof(struct dnshdr)) {
			continue; /* bad size, try again */
		}

		/* Is it my? */
		if (req->msg.hdr.id != rep->msg.hdr.id) {
			continue;
		}

	} while (0);

	/* Close our socket */
	close(sock);

	/* Setup result size */
	*out_rep_sz = (size_t)bytes;

	return 0;
}

static int dns_rr_a_parse(struct dns_rr *curr, const char *data,
		const char *buff, size_t buff_sz) {
	curr->rdata.a.address = *(uint32_t *)data;
	return 0;
}

static int dns_rr_cname_parse(struct dns_rr *curr, const char *data,
		const char *buff, size_t buff_sz) {
	return label_to_name(data, buff, buff_sz, sizeof curr->rdata.cname.cname,
			&curr->rdata.cname.cname[0], NULL);
}

static int dns_result_parse(union dns_msg *dm, size_t dm_sz,
		struct dns_rr **out_result, size_t *out_amount) {
	int ret;
	const char *curr, *start, *end;
	struct dns_rr *arr, *rr;
	size_t i, arr_sz, field_sz;

	if (dm->msg.hdr.qr != DNS_MSG_TYPE_REPLY) {
		return -EINVAL;
	}

	if (dm->msg.hdr.rcode != DNS_RESP_CODE_OK) {
		LOG_ERROR("DNS result code is %d!\n", dm->msg.hdr.rcode);
		return -1;
	}

	arr_sz = ntohs(dm->msg.hdr.ancount);
	arr = malloc(arr_sz * sizeof(struct dns_rr));
	if (arr == NULL) {
		return -ENOMEM;
	}

	curr = &dm->msg.data[0];
	start = &dm->raw[0];
	end = start + dm_sz;
	for (i = 0, rr = arr; i < arr_sz; ++i, ++rr) {

		/* parse name */
		ret = label_to_name(curr, start, end - start,
				sizeof rr->rname, &rr->rname[0], &field_sz);
		if (ret != 0) {
			return ret;
		}
		curr += field_sz;

		/* parse type */
		field_sz = sizeof(uint16_t);
		if (curr + field_sz >= end) {
			return -EINVAL;
		}
		rr->rtype = ntohs(*(uint16_t *)curr);
		curr += field_sz;

		/* parse class */
		field_sz = sizeof(uint16_t);
		if (curr + field_sz >= end) {
			return -EINVAL;
		}
		rr->rclass = (uint16_t)ntohs(*(uint16_t *)curr);
		curr += field_sz;

		/* parse ttl */
		field_sz = sizeof(int16_t);
		if (curr + field_sz >= end) {
			return -EINVAL;
		}
		rr->rttl = ntohs(*(int16_t *)curr);
		curr += field_sz;

		/* parse data length */
		field_sz = sizeof(uint16_t);
		if (curr + field_sz >= end) {
			return -EINVAL;
		}
		rr->rdlength = (uint16_t)ntohs(*(uint16_t *)curr);
		curr += field_sz;

		/* parse data */
		field_sz = rr->rdlength;
		if (curr + field_sz >= end) {
			return -EINVAL;
		}
		switch (rr->rtype) { /* TODO use table of methods instead */
		default:
			ret = -ENOSYS;
			LOG_ERROR("can't parse type %d\n", rr->rtype);
			break;
		case DNS_RR_TYPE_A:
			ret = dns_rr_a_parse(rr, curr, start, end - start);
			break;
		case DNS_RR_TYPE_CNAME:
			ret = dns_rr_cname_parse(rr, curr, start, end - start);
			break;
		}
		if (ret != 0) {
			return ret;
		}
		curr += field_sz;
	}

	return 0;
}

static int dns_execute(struct dns_q *query,
		struct dns_rr **out_results, size_t *out_amount) {
	int ret;
	union dns_msg msg_in, msg_out;
	size_t msg_in_sz, msg_out_sz;

	ret = dns_query_format(query, &msg_out, &msg_out_sz);
	if (ret != 0) {
		return ret;
	}

	ret = dns_query_execute(&msg_out, msg_out_sz, &msg_in, &msg_in_sz);
	if (ret != 0) {
		return ret;
	}

	ret = dns_result_parse(&msg_in, msg_in_sz, out_results, out_amount);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

int dns_query(const char *qname, enum dns_type qtype, enum dns_class qclass,
		struct dns_rr **out_results, size_t *out_amount) {
	struct dns_q query;
	size_t qname_sz;

	qname_sz = strlen(qname) + 1;
	if (qname_sz > sizeof query.qname) {
		return -EINVAL;
	}

	memcpy(&query.qname[0], qname, qname_sz);
	query.qtype = qtype;
	query.qclass = qclass;

	return dns_execute(&query, out_results, out_amount);
}

