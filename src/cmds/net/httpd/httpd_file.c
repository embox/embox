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
#include <net/util/httpd.h>

#define PAGE_INDEX  "index.html"

#ifdef __EMBUILD_MOD__
#	include <framework/mod/options.h>
# define USE_INDEX_AS_404          OPTION_GET(BOOLEAN,use_index_as_404)
#endif /* __EMBUILD_MOD__ */

int httpd_try_respond_file(const struct client_info *cinfo, const struct http_req *hreq,
		char *buf, size_t buf_sz) {
	char path[HTTPD_MAX_PATH];
	char *uri_path;
	size_t read_bytes;
	FILE *file;
	int path_len, retcode, cbyte;

	if (0 == strcmp(hreq->uri.target, "/")) {
		uri_path = PAGE_INDEX;
	} else {
		uri_path = hreq->uri.target;
	}

	path_len = snprintf(path, sizeof(path), "%s/%s", cinfo->ci_basedir, uri_path);
	if (path_len >= sizeof(path)) {
		return -ENOMEM;
	}

	httpd_debug("requested: %s, on fs: %s", hreq->uri.target, path);

	file = fopen(path, "r");
	if (!file) {
#ifdef USE_INDEX_AS_404
		httpd_debug("file doesn't exist, trying index");
		
		uri_path = PAGE_INDEX;
		path_len = snprintf(path, sizeof(path), "%s/%s", cinfo->ci_basedir, uri_path);
		if (path_len >= sizeof(path)) {
			return -ENOMEM;
		}
		
		file = fopen(path, "r");
		if (!file) {
#endif			
			httpd_debug("file couldn't be opened (%d)", errno);
			return 0;
#ifdef USE_INDEX_AS_404
		}
#endif			
	}

	cbyte = snprintf(buf, buf_sz,
			"HTTP/1.1 %d %s\r\n"
			"Content-Type: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			200, "", httpd_filename2content_type(path));

	if (0 > write(cinfo->ci_sock, buf, cbyte)) {
		retcode = -errno;
		goto out;
	}

	retcode = 1;
	while (0 != (read_bytes = fread(buf, 1, buf_sz, file))) {
		const char *pb;
		int remain_send_bytes;

		pb = buf;
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

