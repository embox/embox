/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.04.2015
 */

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "httpd.h"

#define BUFF_SZ 1024
#define PAGE_INDEX  "index.html"

static char httpd_g_outbuf[BUFF_SZ];

int httpd_try_respond_file(const struct client_info *cinfo, const struct http_req *hreq) {
	char path[HTTPD_MAX_PATH];
	char *uri_path;
	size_t read_bytes;
	FILE *file;
	int retcode, cbyte;

	if (0 == strcmp(hreq->uri.target, "/")) {
		uri_path = PAGE_INDEX;
	} else {
		uri_path = hreq->uri.target;
	}

	if (sizeof(path) <= snprintf(path, sizeof(path), "%s/%s", cinfo->ci_basedir, uri_path)) {
		return -ERANGE;
	}

	HTTPD_DEBUG("requested: %s, on fs: %s\n", hreq->uri.target, path);

	file = fopen(path, "r");
	if (!file) {
		HTTPD_DEBUG("%s: file couldn't be opened (%d)\n", __func__, errno);
		return 0;
	}

	cbyte = snprintf(httpd_g_outbuf, sizeof(httpd_g_outbuf),
			"HTTP/1.1 %d %s\r\n"
			"Content-Type: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			200, "", httpd_filename2content_type(path));

	if (0 > write(cinfo->ci_sock, httpd_g_outbuf, cbyte)) {
		retcode = -errno;
		goto out;
	}

	retcode = 1;
	while (0 != (read_bytes = fread(httpd_g_outbuf, 1, sizeof(httpd_g_outbuf), file))) {
		const char *pb;
		int remain_send_bytes;

		pb = httpd_g_outbuf;
		remain_send_bytes = read_bytes;
		while (remain_send_bytes) {
			int sent_bytes;

			if (0 > (sent_bytes = write(cinfo->ci_sock, pb, read_bytes))) {
				retcode = sent_bytes;
				break;
			}

			pb += sent_bytes;
			remain_send_bytes -= sent_bytes;
		}
	}
out:
	fclose(file);
	return retcode;
}

