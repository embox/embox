/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.04.2015
 */

#ifndef HTTPD_H_
#define HTTPD_H_

#include <stdio.h>
#include <sys/socket.h>

#ifdef __EMBUILD_MOD__

#include <util/log.h>
#define httpd_error log_error
#define httpd_warning log_warning
#define httpd_info log_info
#define httpd_debug log_debug

#else /* __EMBUILD_MOD__ */

#include <stdio.h>
#define httpd_error(fmt, ...) fprintf(stderr, "error: " fmt "\n", ##__VA_ARGS__)
#define httpd_warning(fmt, ...) fprintf(stderr, "warning: " fmt "\n", ##__VA_ARGS__)
#define httpd_info(fmt, ...) fprintf(stderr, "info: " fmt "\n", ##__VA_ARGS__)
#define httpd_debug(fmt, ...) fprintf(stderr, "debug: " fmt "\n", ##__VA_ARGS__)

#endif /* __EMBUILD_MOD__ */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) \
	(sizeof(array) / sizeof(*(array)))
#endif

#define HTTPD_MAX_PATH 128

struct client_info {
	struct sockaddr ci_addr;
	socklen_t ci_addrlen;
	int ci_sock;
	int ci_index;

	const char *ci_basedir;
};

struct http_req_uri {
	char *target;
	char *query;
};

struct http_req {
	struct http_req_uri uri;
	char *method;
	char *content_len;
	char *content_type;
	char *authorization;
};

extern char *httpd_parse_request(char *str, struct http_req *hreq);
/* return -errcode or number of read butes */
extern int httpd_build_request(struct client_info *cinfo, struct http_req *req,
		char *buf, size_t buf_sz);

extern pid_t httpd_try_process(const struct client_info *cinfo, const struct http_req *hreq);
extern int httpd_try_respond_file(const struct client_info *cinfo, const struct http_req *hreq,
		char *buf, size_t buf_sz);

extern const char *httpd_filename2content_type(const char *filename);
extern int httpd_header(const struct client_info *cinfo, int st, const char *msg);

#endif /* HTTPD_H_ */


