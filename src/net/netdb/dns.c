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

static int dns_header(char *buff, size_t buff_sz, size_t *out_bytes) {
    struct dnshdr *dnsh;

    /* Set DNS header pointer */
    dnsh = (struct dnshdr *)buff;

    /* Check boundary */
    if (sizeof *dnsh > buff_sz) {
        return -ENOMEM;
    }

	/* Clear header's fields */
	memset(dnsh, 0, sizeof *dnsh);

	/* Setup new values */
	dnsh->id = 0x1234; /* TODO */
	dnsh->qr = DNS_MSG_TYPE_QUERY;
	dnsh->opcode = DNS_OPER_CODE_QUERY;
	dnsh->rd = 1;
	dnsh->qdcount = 1;

    *out_bytes = sizeof *dnsh;

	return 0;
}

static int name_to_label(const char *name, char *buff,
        size_t buff_sz, size_t *out_bytes) {
    char *dot;
    size_t bytes_left, field_sz;

    bytes_left = buff_sz;

    do {
        dot = strchr(name, '.');
        field_sz = (dot != NULL ? dot - name : strlen(name));
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

    *out_bytes = buff_sz - bytes_left;

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

static int dns_query_format(struct dns_q *query, char *buff,
        size_t buff_sz, size_t *out_bytes) {
    int ret;
    size_t bytes_left, bytes;

    bytes_left = buff_sz;

	ret = dns_header(buff, bytes_left, &bytes);
	if (ret != 0) {
		return ret;
	}
    bytes_left -= bytes;
    buff += bytes;

	ret = dns_q_format(query, buff, bytes_left, &bytes);
	if (ret != 0) {
		return ret;
	}
    bytes_left -= bytes;
    buff += bytes;

    *out_bytes = buff_sz - bytes_left;

    return 0;
}

static int dns_query_execute(char *req, size_t req_sz,
        char *out_rep, size_t *out_rep_sz) {
    int sock;
    ssize_t bytes;
    struct sockaddr_in dns_host;

    memset(&dns_host, 0, sizeof dns_host);
    /* setup dns_host structure */

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        return sock;
    }

    bytes = sendto(sock, req, req_sz, 0, (struct sockaddr *)&dns_host, sizeof dns_host);
    if (bytes != req_sz) {
        close(sock);
        return errno ? -errno : -1; /* if errno equal to zero O_o */
    }

    bytes = recvfrom(sock, out_rep, *out_rep_sz, 0, NULL, NULL);
    if (bytes <= 0) {
        close(sock);
        return errno ? -errno : -1; /* if errno equal to zero O_o */
    }

    close(sock);

    *out_rep_sz = (size_t)bytes;

    return -ENOSYS;
}

static int dns_result_parse(char *buff, size_t buff_sz,
		struct dns_rr **out_result, size_t *out_result_amount) {
    return -ENOSYS;
}

static int dns_execute(struct dns_q *query,
		struct dns_rr **out_result, size_t *out_result_amount) {
	int ret;
	char in[DNS_MAX_MESSAGE_SZ], out[DNS_MAX_MESSAGE_SZ];
	size_t in_sz, out_sz;

    ret = dns_query_format(query, &out[0], sizeof out, &out_sz);
    if (ret != 0) {
        return ret;
    }

    ret = dns_query_execute(&out[0], out_sz, &in[0], &in_sz);
    if (ret != 0) {
        return ret;
    }

    ret = dns_result_parse(&in[0], in_sz, out_result, out_result_amount);
    if (ret != 0) {
        return ret;
    }

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

	return dns_execute(&query, out_result, out_amount);
}

