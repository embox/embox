/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2015
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <util/math.h>

#include <lib/md5.h>

#define BUFF_SZ 1024
static char mpfd_g_buf[BUFF_SZ];

struct parseenv {
	char *pb;
	int blen;
};

static char *strstrp(const char *s, const char *h) {
	char *f = strstr(s, h);
	if (f) {
		return f + strlen(h);
	}
	return f;
}

static int mpfd_expect(struct parseenv *pe, const char *s) {
	int slen = strlen(s);
	if (0 != strncmp(pe->pb, s, slen)) {
		return 0;
	}
	pe->pb += slen;
	pe->blen -= slen;
	return 1;
}

static int mpfd_skip_till(struct parseenv *pe, const char *s) {
	char *found = strstrp(pe->pb, s);
	if (found) {
		pe->blen -= found - pe->pb;
		pe->pb = found;
	}
	return !!found;
}

static char *mpfd_store_till(struct parseenv *pe, const char *s, size_t *ssize) {
	char *pb = pe->pb;
	if (!mpfd_skip_till(pe, s)) {
		return NULL;
	}
	if (*ssize) {
		*ssize = pe->pb - pb;
	}
	return pb;
}

int main(int argc, char *argv[]) {
	char *method;

	method = getenv("REQUEST_METHOD");
	if (0 == strcmp("POST", method)) {
		struct parseenv _pe;
		struct parseenv *const pe = &_pe;
		int clen = atoi(getenv("CONTENT_LENGTH"));
		char *bound = strstrp(getenv("CONTENT_TYPE"), "boundary=");
		char *filename;
		size_t filenamelen;
		int res;

		/* not going to read footer */
		clen -= strlen("\r\n--") + strlen(bound) + strlen("--\r\n");

		pe->pb = mpfd_g_buf;
		pe->blen = read(STDIN_FILENO, mpfd_g_buf, min(BUFF_SZ, clen));
		clen -= pe->blen;

		res = mpfd_expect(pe, "--") &&
			mpfd_expect(pe, bound) &&
			mpfd_expect(pe, "\r\nContent-Disposition: form-data; name=\"") &&
			mpfd_store_till(pe, "\"; filename=\"", NULL) &&
			(filename = mpfd_store_till(pe, "\"\r\n", &filenamelen)) &&
			mpfd_skip_till(pe, "\r\n\r\n");

		if (res) {
			md5_state_t state;
			md5_init(&state);
			md5_append(&state, (md5_byte_t *) pe->pb, pe->blen);
			md5_byte_t digest[16];
			int i;

			while (clen > 0) {
				int len = read(STDIN_FILENO, mpfd_g_buf, min(BUFF_SZ, clen));
				md5_append(&state, (md5_byte_t *) mpfd_g_buf, len);
				clen -= len;
			}
			md5_finish(&state, digest);
			for (i = 0; i < sizeof(digest); i++) {
				fprintf(stderr, "%02x", digest[i]);
			}
			fprintf(stderr, "\n");
		} else {
			fprintf(stderr, "parse_error\n");
		}

		printf("HTTP/1.1 %d %s\r\n"
			"Content-Type: %s\r\n"
			"Connection: close\r\n"
			"\r\n", res ? 200 : 500, "OK", "text/plain");
	}

	return 0;
}

