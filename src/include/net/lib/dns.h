/**
 * @file
 * @brief
 * @details RFC 1035 (DOMAIN NAMES - IMPLEMENTATION AND SPECIFICATION)
 *
 * @date 22.08.12
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_DNS_H_
#define NET_LIB_DNS_H_

#include <hal/arch.h>
#include <stdint.h>
#include <stdlib.h>
#include <linux/types.h>
#include <net/l3/ipv4/ip.h>

/**
 * Protocol's specific constants
 */
#define DNS_MAX_LABEL_SZ   64
#define DNS_MAX_NAME_SZ    256
#define DNS_MAX_MESSAGE_SZ 512
#define DNS_PORT_NUMBER    53
#define DNS_LABEL_MASK     0xC0

/**
 * Types of messages
 */
enum dns_msg_type {
	DNS_MSG_TYPE_QUERY = 0, /* a request message */
	DNS_MSG_TYPE_REPLY = 1  /* a reply message */
};

/**
 * Types of operations
 */
enum dns_oper_code {
	DNS_OPER_CODE_QUERY = 0,  /* a standard query */
	DNS_OPER_CODE_IQUERY = 1, /* an inverse query */
	DNS_OPER_CODE_STATUS = 2  /* a server status requset */
};

/**
 * Operation retrun code
 */
enum dns_resp_code {
	DNS_RESP_CODE_OK = 0,      /* ok */
	DNS_RESP_CODE_EFORMAT = 1, /* format error */
	DNS_RESP_CODE_ESERVER = 2, /* server failure */
	DNS_RESP_CODE_NONAME = 3,  /* name error */
	DNS_RESP_CODE_NOIMPL = 4,  /* not implemented */
	DNS_RESP_CODE_REFUSE = 5   /* refused */
};

/**
 * Header for DNS messages
 */
typedef struct dnshdr {
	__be16 id;      /* unique id */
#if  __BYTE_ORDER == __LITTLE_ENDIAN
	__u16 rd:1,
		tc:1,
		aa:1,
		opcode:4,
		qr:1,
		rcode:4,
		z:3,
		ra:1;
#elif  __BYTE_ORDER == __BIG_ENDIAN
	__u16 qr:1,     /* type of the message */
		opcode:4,   /* type of the operation */
		aa:1,       /* authoritative answer */
		tc:1,       /* truncation */
		rd:1,       /* recursion desired */
		ra:1,       /* recursion available */
		z:3,        /* reserved (must be zero) */
		rcode:4;    /* response code */
#endif
	__be16 qdcount; /* number of questions */
	__be16 ancount; /* number of answers */
	__be16 nscount; /* number of authority records */
	__be16 arcount; /* number of additional records */
} __attribute__((packed)) dnshdr_t;

/**
 * DNS Types
 */
enum dns_type {
	/* Resource Records types */
	DNS_RR_TYPE_A = 1,      /* a host address */
	DNS_RR_TYPE_NS = 2,     /* an authoritative name server */
	DNS_RR_TYPE_MD = 3,     /* a mail destination (use MX insted) */
	DNS_RR_TYPE_MF = 4,     /* a mail forwarder (use MX insted) */
	DNS_RR_TYPE_CNAME = 5,  /* the canonical name for an alias */
	DNS_RR_TYPE_SOA = 6,    /* marks the start of a zone of authority */
	DNS_RR_TYPE_MB = 7,     /* a mailbox domain name (experimental type) */
	DNS_RR_TYPE_MG = 8,     /* a mail group member (experimental type) */
	DNS_RR_TYPE_MR = 9,     /* a mail rename domain name (experimental type) */
	DNS_RR_TYPE_NULL = 10,  /* a null RR (experimental type) */
	DNS_RR_TYPE_WKS = 11,   /* a well known service description */
	DNS_RR_TYPE_PTR = 12,   /* a domain name pointer */
	DNS_RR_TYPE_HINFO = 13, /* host information */
	DNS_RR_TYPE_MINFO = 14, /* mailbox or mail list information */
	DNS_RR_TYPE_MX = 15,    /* mail exchange */
	DNS_RR_TYPE_TXT = 16,   /* text strings */
	DNS_RR_TYPE_AAAA = 28,  /* a host IPv6 address */

	/* Questions types */
	DNS_Q_TYPE_AXFR = 252,  /* a request for a transfer of an entire zone */
	DNS_Q_TYPE_MAILB = 253, /* a request for mailbox-related records (MB, MG or MR) */
	DNS_Q_TYPE_MAILA = 254, /* a request for mail agent RRs (use MX instead) */
	DNS_Q_TYPE_ALL = 255    /* a request for all records */
};

/**
 * Resource data formats
 */
struct dns_rr_a {  char address[IP_ADDR_LEN]; }; /* Host address format */
struct dns_rr_ns { char nsdname[DNS_MAX_NAME_SZ]; }; /* Authoritative name server format */
struct dns_rr_md { char madname[DNS_MAX_NAME_SZ]; }; /* Mail destination format */
struct dns_rr_mf { char madname[DNS_MAX_NAME_SZ]; }; /* Mail forwarder format */
struct dns_rr_cname { char cname[DNS_MAX_NAME_SZ]; }; /* Canonical name format */
struct dns_rr_soa { char mname[DNS_MAX_NAME_SZ]; char rname[DNS_MAX_NAME_SZ]; uint32_t serial;
		int32_t refresh; int32_t retry; int32_t expire; int32_t minimum; }; /* SOA mark format */
struct dns_rr_mb { char madname[DNS_MAX_NAME_SZ]; }; /* Mailbox domain name format */
struct dns_rr_mg { char mgmname[DNS_MAX_NAME_SZ]; }; /* Mail group member format */
struct dns_rr_mr { char newname[DNS_MAX_NAME_SZ]; }; /* Mail rename domain name format */
struct dns_rr_null { }; /* NULL RR format */
struct dns_rr_wks { uint32_t address; uint8_t protocol; char *bit_map; }; /* WKS description format */
struct dns_rr_ptr { char ptrdname[DNS_MAX_NAME_SZ]; }; /* Domain name pointer format */
struct dns_rr_hinfo { char *cpu; char *os; }; /* Host information format */
struct dns_rr_minfo { char rmailbx[DNS_MAX_NAME_SZ];
		char emailbx[DNS_MAX_NAME_SZ]; }; /* Mailbox (mail list) information format */
struct dns_rr_mx { int16_t preference; char exchange[DNS_MAX_NAME_SZ]; }; /* Mail exchange format */
struct dns_rr_txt { char *txt_data; }; /* Text strings format */
struct dns_rr_aaaa {  char address[IPv6_ADDR_LEN]; }; /* Host IPv6 address format */

/**
 * DNS Classes
 */
enum dns_class {
	/* Resource Records classes */
	DNS_RR_CLASS_IN = 1, /* the Internet */
	DNS_RR_CLASS_CS = 2, /* the CSNET class */
	DNS_RR_CLASS_CH = 3, /* the CHAOS class */
	DNS_RR_CLASS_HS = 4, /* Hesiod */

	/* Question classes */
	DNS_Q_ANY = 255      /* any class */
};

/**
 * DNS Resource Record header
 */
struct dns_rr {
	char rname[DNS_MAX_NAME_SZ]; /* an owner name */
	uint16_t rtype;              /* type of a record */
	uint16_t rclass;             /* class of a record */
	uint32_t rttl;               /* time life of this record */
	uint16_t rdlength;           /* size of rdata field */
	union {
		struct dns_rr_a a;
		struct dns_rr_ns ns;
		struct dns_rr_md md;
		struct dns_rr_mf mf;
		struct dns_rr_cname cname;
		struct dns_rr_soa soa;
		struct dns_rr_mb mb;
		struct dns_rr_mg mg;
		struct dns_rr_mr mr;
		struct dns_rr_null null;
		struct dns_rr_wks wks;
		struct dns_rr_ptr ptr;
		struct dns_rr_hinfo hinfo;
		struct dns_rr_minfo minfo;
		struct dns_rr_mx mx;
		struct dns_rr_txt txt;
		struct dns_rr_aaaa aaaa;
	} rdata;                    /* resource data */
};

/**
 * DNS Question header
 */
struct dns_q {
	char qname[DNS_MAX_NAME_SZ]; /* domain name */
	uint16_t qtype;              /* type of the query */
	uint16_t qclass;             /* class of the query */
};

/**
 * DNS Result
 */
struct dns_result {
	/* Queries */
	size_t qdcount;
	struct dns_q *qd;
	/* Answers */
	size_t ancount;
	struct dns_rr *an;
	/* Authoitative nameservers */
	size_t nscount;
	struct dns_rr *ns;
	/* Additional records */
	size_t arcount;
	struct dns_rr *ar;
};

/**
 * dns_query - make query with specified type and class
 */
extern int dns_query(const char *query, enum dns_type qtype, enum dns_class qclass,
		struct dns_result *out_result);

/**
 * dns_result_free - free resource from dns_result structure
 */
extern int dns_result_free(struct dns_result *result);

/**
 * dns_get_nameserver - get dns nameserver
 */
extern const char * dns_get_nameserver(void);

#endif /* NET_LIB_DNS_H_ */
