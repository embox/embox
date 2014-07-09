/**
 * @file
 * @brief Simple HTTP server
 * @date 16.04.12
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <net/l3/ipv4/ip.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>

#include <net/inetdevice.h>
#include <embox/cmd.h>
#include <ifaddrs.h>
#include <util/math.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <sys/wait.h>

EMBOX_CMD(httpd);

#define USE_IP_VER 4

#define HTTPD_LOG_QUIET 0
#define HTTPD_LOG_ERROR 1
#define HTTPD_LOG_DEBUG 2

#define HTTPD_LOG_LEVEL HTTPD_LOG_DEBUG

#define BUFF_SZ     512
#define PAGE_INDEX  "index.html"
#define PAGE_4XX    "404.html"
#define CGI_PREFIX  "/cgi-bin/"

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
};

struct http_req_uri {
	char *target;
	char *query;
};

struct http_req {
	struct http_req_uri uri;
	char *method;
	char *http_ver;
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
		HTTPD_ERROR("%s: can't find URI-Version separator\n", __func__);
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
		HTTPD_ERROR("%s: can't parse uri\n", __func__);
		return NULL;
	}

	pb = strstr(pb, "\r\n");
	if (!pb) {
		HTTPD_ERROR("%s: can't find sentinel\n", __func__);
		return NULL;
	}

	return pb + strlen("\r\n");
}

static char *httpd_parse_headers(char *str, void *hheds) {
	char *pb;

	while (0 != strncmp("\r\n", pb, strlen("\r\n"))) {
		pb = strstr(pb, "\r\n");
		if (!pb) {
			HTTPD_ERROR("%s: can't find sentinel\n", __func__);
			return NULL;
		}
		pb += strlen("\r\n");
	}

	return pb + strlen("\r\n");
}

static char *httpd_parse_request(char *str, struct http_req *hreq) {
	char *pb;
	pb = str;

	pb = httpd_parse_request_line(str, hreq);
	if (!pb) {
		HTTPD_ERROR("%s: can't parse request line\n", __func__);
		return NULL;
	}

	return httpd_parse_headers(pb, NULL);
}

static const char *ext2type_html[] = { ".html", ".htm", NULL };
static const char *ext2type_jpeg[] = { ".jpeg", ".jpg", NULL };
static const struct ext2type_table_item {
	const char *type;
	const char *ext;
	const char **exts;
} httpd_ext2type_table[] = {
	{ .exts = ext2type_html, .type = "text/html", },
	{ .exts = ext2type_jpeg, .type = "image/jpeg", },
	{ .ext = ".png",         .type = "image/png", },
	{ .ext = ".gif",         .type = "image/gif", },
	{ .ext = ".ico",         .type = "image/vnd.microsoft.icon", },
	{ .ext = "",             .type = "application/unknown", },
};
static const char *ext2type_unkwown = "plain/text";

static const char *httpd_filename2content_type(const char *filename) {
	int i_table;
	const char *file_ext;

	file_ext = strchrnul(filename, '.');

	for (i_table = 0; i_table < ARRAY_SIZE(httpd_ext2type_table); i_table ++) {
		const struct ext2type_table_item *ti = &httpd_ext2type_table[i_table];

		if (ti->ext) {
			if (0 == strcmp(file_ext, ti->ext)) {
				return ti->type;
			}
		}
		if (ti->exts) {
			const char **ext;
			for (ext = ti->exts; *ext != NULL; ext++) {
				if (0 == strcmp(file_ext, *ext)) {
					return ti->type;
				}
			}
		}
	}

	HTTPD_ERROR("can't determ content type for file: %s\n", filename);
	return ext2type_unkwown;
}

static int httpd_send_response_file(const struct client_info *cinfo, const struct http_req *hreq) {
	char outbuf[BUFF_SZ];
	const char *filename;
	FILE *file;
	int status, cbyte;
	size_t read_bytes;

	filename = hreq->uri.target;

	/* TODO not every file is intended to be published, make it serve only one
 	 * directory, not a whole root.
	 */
	file = fopen(filename, "r");
	if (!file) {
		filename = PAGE_4XX;
		file = fopen(filename, "r");
		/* testing file for NULL performed later */
		status = 404;
	} else {
		status = 200;
	}

	cbyte = snprintf(outbuf, sizeof(outbuf),
			"HTTP/1.1 %d %s\r\n"
			"Content-Type: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			status, "", httpd_filename2content_type(filename));

	if (0 > write(cinfo->ci_sock, outbuf, cbyte)) {
		return -errno;
	}

	if (!file) { /* file could be NULL if wasn't found error template.
			In this case send header only, body is empty */
		return 0;
	}

	while (0 != (read_bytes = fread(outbuf, 1, sizeof(outbuf), file))) {
		const char *pb;
		int remain_send_bytes;

		pb = outbuf;
		remain_send_bytes = read_bytes;
		while (remain_send_bytes) {
			int sent_bytes;

			if (0 > (sent_bytes = write(cinfo->ci_sock, pb, read_bytes))) {
				return -errno;
			}

			pb += sent_bytes;
			remain_send_bytes -= sent_bytes;
		}
	}

	fclose(file);
	return 0;
}

static int httpd_send_response_cgi(const struct client_info *cinfo, const struct http_req *hreq) {
	FILE *skf;

       	skf = fdopen(cinfo->ci_sock, "rw");
	if (!skf) {
		HTTPD_ERROR("can't allocate FILE for socket");
		return -ENOMEM;
	}

	fprintf(skf,
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Connection: close\r\n"
		"\r\n"
		"%s", 200, "OK", "text/plain", "Sorry, CGI is NYI");

	fclose(skf);

	return 0;
}

#if 0
static int httpd_send_response_cgi(const struct client_info *cinfo, const struct http_req *hreq) {
	const char *cmdname;
	pid_t pid;

	cmdname = hreq->uri.target;
	if (0 == strncmp(cmdname, CGI_PREFIX, strlen(CGI_PREFIX))) {
		cmdname += strlen(CGI_PREFIX);
	}

	pid = vfork();
	if (pid < 0) {
		return pid;
	}

	if (pid == 0) {
		char *argv[] = { NULL, };

		close(STDIN_FILENO); /* TODO POST data goes here, actually */
		dup2(cinfo->ci_sock, STDOUT_FILENO);
		dup2(cinfo->ci_sock, STDERR_FILENO);
		execv(cmdname, argv);
		exit(1);
	} else {
		while (pid != waitpid(pid, NULL, 0));
	}

	return 0;
}
#endif

static int httpd_client_process(const struct client_info *cinfo) {
	char buf[BUFF_SZ];
	struct http_req hreq;
	const int sk = cinfo->ci_sock;
	int ret;

	ret = read(sk, buf, sizeof(buf));
	if (ret < 0) {
		HTTPD_ERROR("can't read from client socket: %s", strerror(errno));
		return ret;
	}
	buf[ret] = '\0';

	if (NULL == httpd_parse_request(buf, &hreq)) {
		return -EINVAL;
	}

	HTTPD_DEBUG("%s: method=%s uri_target=%s uri_query=%s\n", __func__,
			hreq.method,
			hreq.uri.target,
			hreq.uri.query);

	if (0 == strcmp(hreq.uri.target, "/")) {
		hreq.uri.target = PAGE_INDEX;
	}

	if (0 == strncmp(hreq.uri.target, CGI_PREFIX, strlen(CGI_PREFIX))) {
		return httpd_send_response_cgi(cinfo, &hreq);
	}

	return httpd_send_response_file(cinfo, &hreq);
}

static int httpd(int argc, char **argv) {
	int host;
#if USE_IP_VER == 4
	struct sockaddr_in inaddr;
	const size_t inaddrlen = sizeof(inaddr);
	const int family = AF_INET;

	inaddr.sin_family = AF_INET;
	inaddr.sin_port= htons(80);
	inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
#else /* USE_IP_VER == 6 */
	struct sockaddr_in6 inaddr;
	const size_t inaddrlen = sizeof(inaddr);
	const int family = AF_INET6;

	inaddr.sin6_family = AF_INET6;
	inaddr.sin6_port= htons(80);
	memcpy(&inaddr.sin6_addr, &in6addr_any, sizeof(inaddr.sin6_addr));
#endif

	host = socket(family, SOCK_STREAM, IPPROTO_TCP);
	if (host == -1) {
		HTTPD_ERROR("socket() failure: %s", strerror(errno));
		return -errno;
	}

	if (-1 == bind(host, (struct sockaddr *) &inaddr, inaddrlen)) {
		HTTPD_ERROR("bind() failure: %s", strerror(errno));
		close(host);
		return -errno;
	}

	if (-1 == listen(host, 3)) {
		HTTPD_ERROR("listen() failure: %s", strerror(errno));
		close(host);
		return -errno;
	}

	while (1) {
		struct client_info ci;

		ci.ci_addrlen = inaddrlen;
		ci.ci_sock = accept(host, &ci.ci_addr, &ci.ci_addrlen);
		if (ci.ci_sock == -1) {
			HTTPD_ERROR("accept() failure: %s", strerror(errno));
			continue;
		}
		assert(ci.ci_addrlen == inaddrlen);

		httpd_client_process(&ci);

		close(ci.ci_sock);
	}

	close(host);

	return 0;
}
