/**
 * @file
 * @brief SNTP implementation over IPv4
 *
 * @date 13.07.2012
 * @author Alexander Kalmuk
 */

#include <net/skbuff.h>
#include <net/inet_sock.h>
#include <net/udp.h>
#include <net/ip.h>
#include <net/ntp.h>
#include <errno.h>
#include <kernel/time/time.h>
#include <string.h>

static char kod[3][4] = {"DENY", "RSTR", "RATE"};

static struct ntphdr ntp_config;
/* socket binded to port 123 */
static int ntp_sock;

static inline void ntp_data_ntohs(struct s_ntpdata *data) {
	data->sec = ntohs(data->sec);
	data->fraction = ntohs(data->fraction);
}

static inline void ntp_data_ntohl(struct l_ntpdata *data) {
	data->sec = ntohl(data->sec);
	data->fraction = ntohl(data->fraction);
}

static void ntp_ntoh(struct ntphdr *ntp) {
	ntp->refid = ntohl(ntp->refid);
	ntp_data_ntohs(&ntp->rootdelay);
	ntp_data_ntohs(&ntp->rootdisp);
	ntp_data_ntohl(&ntp->org_ts);
	ntp_data_ntohl(&ntp->rec_ts);
	ntp_data_ntohl(&ntp->ref_ts);
	ntp_data_ntohl(&ntp->xmt_ts);
}

int ntp_client_xmit(int sock, struct sockaddr_in *dst) {
	struct ntphdr x;
	struct timespec ts;

	x.status = NTP_LI_NO_WARNING | NTP_V_4 | NTP_CLIENT;
	gettimeofday(&ts, NULL);
	x.xmt_ts.sec = htonl(ts.tv_sec);
	x.xmt_ts.fraction = htonl((ts.tv_nsec / 232) * 1000);

	return sendto(sock, (void*) &x, sizeof(x), 0, (struct sockaddr *)dst, sizeof(*dst));
}

static int ntp_server_reply(struct ntphdr *r, int mode, struct sockaddr_in *dst) {
	struct ntphdr x;
	struct timespec ts;

	x.status = NTP_SERVER;
	x.status |= get_version(r);
	x.status |= get_mode(&ntp_config);

	if (ntp_config.stratum > NTP_SERVER_UNSYNC)
		x.stratum = 0;
	else
		x.stratum = ntp_config.stratum;

	x.poll = r->poll;
	x.precision = ntp_config.precision;
	x.refid = ntp_config.refid;
	x.ref_ts = ntp_config.ref_ts;
	x.org_ts = r->xmt_ts;
	gettimeofday(&ts, NULL);
	x.xmt_ts.sec = htonl(ts.tv_sec);
	x.xmt_ts.fraction = htonl((ts.tv_nsec / 232) * 1000);

	return sendto(ntp_sock, (void*) &x, sizeof(x), 0, (struct sockaddr *)dst, sizeof(*dst));
}

int ntp_receive(struct sock *sk, struct sk_buff *skb) {
	struct ntphdr *r;
	int mode, res = NET_RX_DROP;
	iphdr_t *iph = ip_hdr(skb);
	udphdr_t *udph = udp_hdr(skb);
	struct inet_sock *inet = inet_sk(sk);

	/* check for NTP packet */
	if (ntohs(inet->dport) != NTP_SERVER_PORT &&
			ntohs(udph->dest) != NTP_SERVER_PORT)
		goto free_and_drop;

	r = (struct ntphdr*)(skb->h.raw + UDP_HEADER_SIZE);

	ntp_ntoh(r);

	if ((mode = get_mode(r)) == NTP_CLIENT || mode == NTP_BROADCAST_CLIENT) {
		struct sockaddr_in dst;
		dst.sin_port = udph->source;
		dst.sin_addr.s_addr = iph->daddr;
		ntp_server_reply(r, NTP_SERVER, &dst);
		return ENOERR;
	}

	if (r->stratum >= NTP_SERVER_UNSYNC ||
			(mode != NTP_SERVER && mode != NTP_BROADCAST))
		goto free_and_drop;

	/* check for server KoD */
	if (r->stratum == 0) {
		if (strcmp((char *)&r->refid, kod[0])) {
			res = NTP_DENY;
		} else if (strcmp((char *)&r->refid, kod[1])) {
			res = NTP_RSTR;
		} else if (strcmp((char *)&r->refid, kod[2])) {
			res = NTP_RATE;
		}
		res = NTP_IGNORED;
		goto free_and_drop;
	}

	return ENOERR;
	/* TODO check Key Identifier (optional) */
free_and_drop:
	skb_free(skb);
	return -res;
}

void ntp_format_to_timespec(struct timespec *ts, struct l_ntpdata ld) {
	ts->tv_sec = ld.sec;
	ts->tv_nsec = (ld.fraction / 1000) * 232;
}

struct timespec ntp_delay(struct ntphdr *ntp) {
	struct timespec client_r, server_x, server_r, client_x, res;

	gettimeofday(&client_r, NULL);
	ntp_format_to_timespec(&client_x, ntp->org_ts);
	ntp_format_to_timespec(&server_x, ntp->xmt_ts);
	ntp_format_to_timespec(&server_r, ntp->rec_ts);

	res = timespec_sub(client_r, client_x);
	res = timespec_sub(res, timespec_sub(server_x, server_r));

	res.tv_sec /= 2;
	res.tv_nsec /= 2;

	return res;
}

int ntp_offset(struct ntphdr *ntp) {
	return 0;
}
