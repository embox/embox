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
#define PAGE_4XX    "404.html"

static char httpd_g_outbuf[BUFF_SZ];

int httpd_send_response_file(const struct client_info *cinfo, const struct http_req *hreq) {
	char filename[HTTPD_MAX_PATH];
	FILE *file;
	int status, cbyte;
	size_t read_bytes;

	snprintf(filename, sizeof(filename), "%s/%s", cinfo->ci_basedir, hreq->uri.target);
	filename[sizeof(filename) - 1] = '\0';

	HTTPD_DEBUG("requested: %s, on fs: %s\n", hreq->uri.target, filename);

	file = fopen(filename, "r");
	if (!file) {
		HTTPD_DEBUG("%s: file couldn't be opened (%d)\n", __func__, errno);
		strcpy(filename, PAGE_4XX);
		file = fopen(filename, "r");
		/* testing file for NULL performed later */
		status = 404;
	} else {
		status = 200;
	}

	cbyte = snprintf(httpd_g_outbuf, sizeof(httpd_g_outbuf),
			"HTTP/1.1 %d %s\r\n"
			"Content-Type: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			status, "", httpd_filename2content_type(filename));

	if (0 > write(cinfo->ci_sock, httpd_g_outbuf, cbyte)) {
		return -errno;
	}

	if (!file) { /* file could be NULL if wasn't found error template.
			In this case send header only, body is empty */
		return 0;
	}

	while (0 != (read_bytes = fread(httpd_g_outbuf, 1, sizeof(httpd_g_outbuf), file))) {
		const char *pb;
		int remain_send_bytes;

		pb = httpd_g_outbuf;
		remain_send_bytes = read_bytes;
		while (remain_send_bytes) {
			int sent_bytes;

			if (0 > (sent_bytes = write(cinfo->ci_sock, pb, read_bytes))) {
				break;
			}

			pb += sent_bytes;
			remain_send_bytes -= sent_bytes;
		}
	}

	fclose(file);
	return 0;
}

