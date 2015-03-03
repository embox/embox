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
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <util/math.h>
#include <framework/mod/options.h>

#define MPFD_BASE OPTION_STRING_GET(base)
#define MPFD_BASE_LEN strlen(MPFD_BASE)

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

static char *mpfd_store_till(struct parseenv *pe, const char *s, size_t *ssize) {
	char *found = strstr(pe->pb, s);
	char *pb = pe->pb;
	int slen = strlen(s);

	if (!found) {
		return NULL;
	}

	if (*ssize) {
		*ssize = found - pb;
	}

	pe->blen -= found - pb + slen;
	pe->pb = found + slen;

	return pb;
}

static int mpfd_skip_till(struct parseenv *pe, const char *s) {
	return !!mpfd_store_till(pe, s, NULL);
}

static int mpfd_parse(struct parseenv *pe, const char *bound, char *fname, size_t fname_len) {
	char *filename;
	size_t filenamelen;
	int pres = mpfd_expect(pe, "--") &&
		mpfd_expect(pe, bound) &&
		mpfd_expect(pe, "\r\nContent-Disposition: form-data; name=\"") &&
		mpfd_store_till(pe, "\"; filename=\"", NULL) &&
		(filename = mpfd_store_till(pe, "\"\r\n", &filenamelen)) &&
		mpfd_skip_till(pe, "\r\n\r\n");

	if (pres) {
		*(filename + filenamelen) = '\0';
		strncat(fname, filename, fname_len);
	}

	return pres;
}

int main(int argc, char *argv[]) {
	char *method;

	method = getenv("REQUEST_METHOD");
	if (0 == strcmp("POST", method)) {
		struct parseenv _pe;
		char *bound = strstrp(getenv("CONTENT_TYPE"), "boundary=");
		int clen = atoi(getenv("CONTENT_LENGTH"));
		char path[64];
		int httpcode;
		int fd;

		/* not going to read footer */
		clen -= strlen("\r\n--") + strlen(bound) + strlen("--\r\n");

		_pe.pb = mpfd_g_buf;
		_pe.blen = read(STDIN_FILENO, mpfd_g_buf, min(BUFF_SZ, clen));
		clen -= _pe.blen;
		strncpy(path, MPFD_BASE, sizeof(path));

		if (!mpfd_parse(&_pe, bound, path + MPFD_BASE_LEN, sizeof(path) - MPFD_BASE_LEN)) {
			fprintf(stderr, "parse_error\n");
			httpcode = 500;
			goto out_header;
		}

		fd = open(path, O_WRONLY | O_CREAT, 0755);
		if (fd < 0) {
			fprintf(stderr, "can't open output file %s: %s\n", path, strerror(errno));
			httpcode = 500;
			goto out_header;
		}

		write(fd, _pe.pb, _pe.blen);
		while (clen > 0) {
			int len = read(STDIN_FILENO, mpfd_g_buf, min(BUFF_SZ, clen));
			write(fd, mpfd_g_buf, len);
			clen -= len;
		}
		close(fd);

		httpcode = 200;
out_header:
		printf("HTTP/1.1 %d %s\r\n"
			"Content-Type: %s\r\n"
			"Connection: close\r\n"
			"\r\n", httpcode, "OK", "text/plain");
		fflush(stdout);
	}

	return 0;
}
