/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.04.2015
 */

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <net/util/httpd.h>

static const struct http_header_desc {
	char *name;
	off_t hreq_offset;
} http_headers[] = {
	{ .name = "Content-Length: ", .hreq_offset = offsetof(struct http_req, content_len), },
	{ .name = "Content-Type: ", .hreq_offset = offsetof(struct http_req, content_type), },
	{ .name = "Authorization: ", .hreq_offset = offsetof(struct http_req, authorization ), },
};

static char *httpd_parse_uri(char *str, struct http_req_uri *huri) {
	char *pb;
	pb = str;

	huri->target = pb;

	pb = strchr(pb, '?');
	if (pb) {
		*(pb++) = '\0';
		huri->query = pb;
	} else {
		pb = huri->target;
		huri->query = NULL;
	}

	pb = strchr(pb, ' ');
	if (!pb) {
		httpd_error("can't find URI-Version separator");
		return NULL;
	}

	*(pb++) = '\0';
	return pb;
}

static char *httpd_parse_request_line(char *str, struct http_req *hreq) {
	char *pb;
	pb = str;

	hreq->method = pb;
	pb = strchr(pb, ' ');
	if (!pb) {
		return NULL;
	}
	*(pb++) = '\0';

	pb = httpd_parse_uri(pb, &hreq->uri);
	if (!pb) {
		httpd_error("can't parse uri");
		return NULL;
	}

	pb = strstr(pb, "\r\n");
	if (!pb) {
		httpd_error("can't find sentinel");
		return NULL;
	}

	return pb + strlen("\r\n");
}

static char *httpd_parse_headers(char *str, struct http_req *hreq) {
	char *pb;

	pb = str;
	while (0 != strncmp("\r\n", pb, strlen("\r\n"))) {
		int i_hh;
		bool found;

		found = false;
		for (i_hh = 0; i_hh < ARRAY_SIZE(http_headers); i_hh++) {
			const struct http_header_desc *hh_d = &http_headers[i_hh];
			size_t len = strlen(hh_d->name);

			if (0 == strncmp(hh_d->name, pb, len)) {

				*(char **) ((void *) hreq + hh_d->hreq_offset) = pb + len;

				pb = strstr(pb + len, "\r\n");
				*pb = '\0';
				pb += strlen("\r\n");
				found = true;
				break;
			}
		}

		if (!found) {
			pb = strstr(pb, "\r\n") + strlen("\r\n");
		}
	}

	return pb + strlen("\r\n");
}

char *httpd_parse_request(char *str, struct http_req *hreq) {
	char *pb;

	pb = httpd_parse_request_line(str, hreq);
	if (!pb) {
		httpd_error("can't parse request line");
		return NULL;
	}

	return httpd_parse_headers(pb, hreq);
}
