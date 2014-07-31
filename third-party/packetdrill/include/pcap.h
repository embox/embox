/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.07.2014
 */

#ifndef PACKEDRILL_PCAP_H_
#define PACKEDRILL_PCAP_H_

typedef	int bpf_int32;
typedef	u_int bpf_u_int32;

typedef int pcap_t;

struct bpf_program {
	u_int bf_len;
	struct bpf_insn *bf_insns;
};

struct pcap_pkthdr {
	struct timeval ts;	/* time stamp */
	bpf_u_int32 caplen;	/* length of portion present */
	bpf_u_int32 len;	/* length this packet (off wire) */
};

#define PCAP_ERRBUF_SIZE 128
#define BIOCIMMEDIATE (PD_STUB("BIOCIMMEDIATE"), 0)

#define DLT_NULL	0	/* BSD loopback encapsulation */
#define DLT_EN10MB	1	/* Ethernet (10Mb) */
#define DLT_EN3MB	2	/* Experimental Ethernet (3Mb) */
#define DLT_AX25	3	/* Amateur Radio AX.25 */
#define DLT_PRONET	4	/* Proteon ProNET Token Ring */
#define DLT_CHAOS	5	/* Chaos */
#define DLT_IEEE802	6	/* 802.5 Token Ring */
#define DLT_ARCNET	7	/* ARCNET, with BSD-style header */
#define DLT_SLIP	8	/* Serial Line IP */
#define DLT_PPP		9	/* Point-to-point Protocol */
#define DLT_FDDI	10	/* FDDI */

#define DLT_LOOP	12
#define DLT_RAW		14	/* raw IP */

#include <stdio.h>
static inline void pcap_perror(pcap_t *pcap, char *msg) {
	fprintf(stderr, "pcap: %s\n", msg);
}

static inline pcap_t *pcap_create(const char *source, char *errbuf) {
	PD_STUB_USE();
	return NULL;
}

static inline int pcap_set_snaplen(pcap_t *p, int snaplen) {
	PD_STUB_USE();
	return 0;
}

static inline int pcap_activate(pcap_t *p) {
	PD_STUB_USE();
	return 0;
}

static inline int pcap_get_selectable_fd(pcap_t *p) {
	PD_STUB_USE();
	return 0;
}

static inline int pcap_datalink(pcap_t *p) {
	PD_STUB_USE();
	return 0;
}

static inline int pcap_compile(pcap_t *p, struct bpf_program *fp,
		const char *str, int optimize, bpf_u_int32 netmask) {
	PD_STUB_USE();
	return 0;
}

static inline int pcap_setfilter(pcap_t *p, struct bpf_program *fp) {
	PD_STUB_USE();
	return 0;
}

static inline void pcap_close(pcap_t *p) {
	PD_STUB_USE();
}

static inline int pcap_inject(pcap_t *p, const void *buf, size_t size) {
	PD_STUB_USE();
	return 0;
}

static inline void pcap_freecode(struct bpf_program *prg) {
	PD_STUB_USE();
}

static inline int pcap_next_ex(pcap_t *p, struct pcap_pkthdr **pkt_header, const u_char **pkt_data) {
	PD_STUB_USE();
	return 0;
}


#endif /* PACKEDRILL_PCAP_H_ */
