/**
 * @file
 * @brief Simple Network Time Protocol (SNTP) Version 4
 *
 * @date 13.07.12
 * @author Alexander Kalmuk
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <kernel/time/time.h>
#include <net/lib/ntp.h>
#include <stddef.h>
#include <string.h>
#include <lib/libds/array.h>

/* RFC 868 */
#define SECONDS_1900_1970 2208988800L

#define NTP_BUILD_DATA(field, data)              \
	do {                                         \
		const void *data_ = data;                \
		if (data != NULL) {                      \
			memcpy(&field, data_, sizeof field); \
		}                                        \
		else {                                   \
			memset(&field, 0, sizeof field);     \
		}                                        \
	} while (0)

int ntp_build(struct ntphdr *ntph, int leap, int mode,
		int stratum, int poll, int precision,
		struct ntp_data_l *ref, struct ntp_data_l *org,
		struct ntp_data_l *rec, struct ntp_data_l *xmt) {
	if (ntph == NULL) {
		return -EINVAL;
	}

	ntph->leap = leap;
	ntph->version = NTP_VERSION;
	ntph->mode = mode;
	ntph->poll = poll;
	ntph->precision = precision;
	NTP_BUILD_DATA(ntph->rootdelay, NULL);
	NTP_BUILD_DATA(ntph->rootdisp, NULL);
	NTP_BUILD_DATA(ntph->reftime, ref);
	NTP_BUILD_DATA(ntph->org, org);
	NTP_BUILD_DATA(ntph->rec, rec);
	NTP_BUILD_DATA(ntph->xmt, xmt);

	return 0;
}

static void ndl_to_ts(const void *in, struct timespec *out_ts) {
	struct ntp_data_l ndl;

	assert(in != NULL);
	assert(out_ts != NULL);

	/* This is a workaround to avoid accessing unaligned data */
	memcpy(&ndl, in, sizeof(ndl));

	out_ts->tv_sec = ntohl(ndl.sec) - SECONDS_1900_1970;
	out_ts->tv_nsec = (ntohl(ndl.frac) / 1000) * 232;
}

int ntp_data_l_to_timespec(const struct ntp_data_l *ndl,
		struct timespec *out_ts) {
	if ((ndl == NULL) || (out_ts == NULL)) {
		return -EINVAL;
	}

	ndl_to_ts(ndl, out_ts);

	return 0;
}

static void ts_to_ndl(const struct timespec *ts,
		struct ntp_data_l *out_ndl) {
	assert(ts != NULL);
	assert(out_ndl != NULL);
	out_ndl->sec = htonl((__be32)ts->tv_sec + SECONDS_1900_1970);
	out_ndl->frac = htonl((ts->tv_nsec / 232) * 1000);
}

int ntp_timespec_to_data_l(const struct timespec *ts,
		struct ntp_data_l *out_ndl) {
	if ((ts == NULL) || (out_ndl == NULL)) {
		return -EINVAL;
	}

	ts_to_ndl(ts, out_ndl);

	return 0;
}

int ntp_mode_client(const struct ntphdr *ntph) {
	if (ntph == NULL) {
		return 0;
	}

	return (ntph->mode == NTP_MOD_CLIENT)
			|| (ntph->mode == NTP_MOD_BROADCAST_CLIENT);
}

int ntp_mode_server(const struct ntphdr *ntph) {
	if (ntph == NULL) {
		return 0;
	}

	return (ntph->mode == NTP_MOD_SERVER)
			|| (ntph->mode == NTP_MOD_BROADCAST);
}

const char * ntp_stratum_error(const struct ntphdr *ntph) {
	static const struct {
		const char *code;
		const char *info;
	} errors[] = {
		{ "ACST", "The association belongs to a anycast server" },
		{ "AUTH", "Server authentication failed" },
		{ "AUTO", "Autokey sequence failed" },
		{ "BCST", "The association belongs to a broadcast server" },
		{ "CRYP", "Cryptographic authentication or identification"
				" failed" },
		{ "DENY", "Access denied by remote server" },
		{ "DROP", "Lost peer in symmetric mode" },
		{ "RSTR", "Access denied due to local policy" },
		{ "INIT", "The association has not yet synchronized for"
				" the first time" },
		{ "MCST", "The association belongs to a manycast server" },
		{ "NKEY", "No key found. Either the key was never"
				" installed or is not trusted" },
		{ "RATE", "Rate exceeded. The server has temporarily"
				" denied access because the client exceeded the"
				" rate threshold" },
		{ "RMOT", "Somebody is tinkering with the association"
				" from a remote host running ntpdc. Not to worry"
				" unless some rascal has stolen your keys" },
		{ "STEP", "A step change in system time has occurred,"
				" but the association has not yet resynchronized" }
	};
	size_t i;

	if (ntph->stratum != NTP_STRATUM_UNSPEC) {
		return NULL;
	}

	for (i = 0; i < ARRAY_SIZE(errors); ++i) {
		if (0 == strncmp(&ntph->refid[0], errors[i].code,
					ARRAY_SIZE(ntph->refid))) {
			return errors[i].info;
		}
	}

	return NULL;
}

int ntp_valid_stratum(const struct ntphdr *ntph) {
	if (ntph == NULL) {
		return 0;
	}
	else if (ntp_mode_client(ntph)) {
		return 1;
	}

	return (ntph->stratum > NTP_STRATUM_UNSPEC)
			&& (ntph->stratum < NTP_STRATUM_UNSYNC);
}

int ntp_delay(const struct ntphdr *ntph,
		struct timespec *recv_time, struct timespec *out_ts) {
	struct timespec client_x, server_r, server_x;

	if ((ntph == NULL) || (out_ts == NULL)) {
		return -EINVAL;
	}

	ndl_to_ts(&ntph->org, &client_x);
	ndl_to_ts(&ntph->rec, &server_r);
	ndl_to_ts(&ntph->xmt, &server_x);

	*out_ts = timespec_sub(timespec_sub(server_r, client_x),
			timespec_sub(server_x, *recv_time));

	return 0;
}

int ntp_offset(const struct ntphdr *ntph,
		struct timespec *recv_time, struct timespec *out_ts) {
	struct timespec client_x, server_r, server_x;

	if ((ntph == NULL) || (out_ts == NULL)) {
		return -EINVAL;
	}

	ndl_to_ts(&ntph->org, &client_x);
	ndl_to_ts(&ntph->rec, &server_r);
	ndl_to_ts(&ntph->xmt, &server_x);

	*out_ts = timespec_add(timespec_sub(server_r, client_x),
			timespec_sub(server_x, *recv_time));
	out_ts->tv_sec /= 2;
	out_ts->tv_nsec /= 2;

	return 0;
}

int ntp_time(const struct ntphdr *ntph, struct timespec *out_ts) {
	int ret;
	struct timespec xmt_time, recv_time, delay;

	if ((ntph == NULL) || (out_ts == NULL)) {
		return -EINVAL;
	}

	ndl_to_ts(&ntph->xmt, &xmt_time);
	getnsofday(&recv_time, NULL);

	ret = ntp_delay(ntph, &recv_time, &delay);
	if (ret != 0) {
		return ret;
	}

	*out_ts = timespec_add(xmt_time, delay);

	return 0;
}
