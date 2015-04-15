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

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) \
	(sizeof(array) / sizeof(*(array)))
#endif

#define HTTPD_MAX_PATH 128
#define CGI_PREFIX  "/cgi-bin/"

#define HTTPD_LOG_QUIET 0
#define HTTPD_LOG_ERROR 1
#define HTTPD_LOG_DEBUG 2

#define HTTPD_LOG_LEVEL HTTPD_LOG_DEBUG

#define HTTPD_L       "httpd: "
#define HTTPD_L_DEBUG HTTPD_L "debug: "

#if HTTPD_LOG_LEVEL >= HTTPD_LOG_DEBUG
#define HTTPD_DEBUG(_msg, ...) \
	fprintf(stderr, HTTPD_L_DEBUG _msg, ## __VA_ARGS__)
#else
#define HTTPD_DEBUG(_msg, ...)
#endif

#if HTTPD_LOG_LEVEL >= HTTPD_LOG_ERROR
#define HTTPD_ERROR(_msg, ...) \
	fprintf(stderr, HTTPD_L _msg, ## __VA_ARGS__)
#else
#define HTTPD_ERROR(_msg, ...)
#endif

struct client_info {
	struct sockaddr ci_addr;
	socklen_t ci_addrlen;
	int ci_sock;

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
};

extern char *httpd_parse_request(char *str, struct http_req *hreq);

extern int httpd_try_response_script(const struct client_info *cinfo, const struct http_req *hreq);
extern int httpd_try_response_cmd(const struct client_info *cinfo, const struct http_req *hreq);
extern int httpd_try_response_file(const struct client_info *cinfo, const struct http_req *hreq);

extern const char *httpd_filename2content_type(const char *filename);

#endif /* HTTPD_H_ */


