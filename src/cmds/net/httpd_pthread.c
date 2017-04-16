/**
 * @file
 * @brief Simple HTTP server
 * @date 16.04.12
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#define _GNU_SOURCE

#include <stdbool.h>
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

#include <framework/mod/options.h>

#include "pthread.h"


#define USE_IP_VER OPTION_GET(NUMBER,use_ip_ver)
#define USE_CGI    OPTION_GET(BOOLEAN,use_cgi)


#define HTTPD_LOG_QUIET 0
#define HTTPD_LOG_ERROR 1
#define HTTPD_LOG_DEBUG 2

#define HTTPD_LOG_LEVEL HTTPD_LOG_QUIET

#define HTTPD_MAX_PATH 128
#define BUFF_SZ     1024
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

#define ARRAY_SIZE(array) \
	(sizeof(array) / sizeof(*(array)))

#define MAX_CLIENTS_COUNT 4

struct client_info {
	struct sockaddr ci_addr;
	socklen_t ci_addrlen;
	int ci_sock;
        int ci_index;
        int id;

	const char *ci_basedir;
};
struct my{
	const char *basedir;
	int host;
};

struct http_req_uri {
	char *target;
	char *query;
};

struct http_req {
	struct http_req_uri uri;
	char *method;
	char *http_ver;
	char *content_len;
};




static struct client_info clients[MAX_CLIENTS_COUNT];
static char client_is_free[MAX_CLIENTS_COUNT];

static int clients_get_free_index(void) {
	int i;

	for (i = 0; i < MAX_CLIENTS_COUNT; i++) {
		if (client_is_free[i]) {
			client_is_free[i] = 0;
			return i;
		}
	}
	return -1; // there are no free indexes
}

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

static const struct http_header_desc {
	char *name;
	off_t hreq_offset;
} http_headers[] = {
	{ .name = "Content-Length: ", .hreq_offset = offsetof(struct http_req, content_len), },
};
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

static char *httpd_parse_request(char *str, struct http_req *hreq) {
	char *pb;
	pb = str;

	pb = httpd_parse_request_line(str, hreq);
	if (!pb) {
		HTTPD_ERROR("%s: can't parse request line\n", __func__);
		return NULL;
	}

	return httpd_parse_headers(pb, hreq);
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
	{ .ext = ".js",          .type = "application/javascript", },
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
	char filename[HTTPD_MAX_PATH];
	FILE *file;
	int status, cbyte;
	size_t read_bytes;
        char httpd_outbuf[BUFF_SZ];

	snprintf(filename, sizeof(filename), "%s/%s", cinfo->ci_basedir, hreq->uri.target);
	filename[sizeof(filename) - 1] = '\0';

	HTTPD_DEBUG("requested: %s, on fs: %s\n", hreq->uri.target, filename);

	file = fopen(filename, "r");
	if (!file) {
		strcpy(filename, PAGE_4XX);
		file = fopen(filename, "r");
		/* testing file for NULL performed later */
		status = 404;
	} else {
		status = 200;
	}

	cbyte = snprintf(httpd_outbuf, sizeof(httpd_outbuf),
			"HTTP/1.1 %d %s\r\n"
			"Content-Type: %s\r\n"
			"Connection: close\r\n"
			"\r\n",
			status, "", httpd_filename2content_type(filename));

	if (0 > write(cinfo->ci_sock, httpd_outbuf, cbyte)) {
		return -errno;
	}

	if (!file) { /* file could be NULL if wasn't found error template.
			In this case send header only, body is empty */
		return 0;
	}

	while (0 != (read_bytes = fread(httpd_outbuf, 1, sizeof(httpd_outbuf), file))) {
		const char *pb;
		int remain_send_bytes;

		pb = httpd_outbuf;
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

#if !USE_CGI
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
		"%s", 200, "OK", "text/plain",
		"Sorry, CGI support is disabled");

	fclose(skf);

	return 0;
}

#else

static const struct cgi_env_descr {
	char *name;
	off_t hreq_offset;
} cgi_env[] = {
	{ .name = "REQUEST_METHOD", .hreq_offset = offsetof(struct http_req, method) },
	{ .name = "CONTENT_LENGTH", .hreq_offset = offsetof(struct http_req, content_len) },
};
static int httpd_send_response_cgi(const struct client_info *cinfo, const struct http_req *hreq) {
	char *cmdname;
	pid_t pid;
	struct stat fstat;
	FILE *skf;

	cmdname = hreq->uri.target;
	if (stat(cmdname, &fstat)) {
		/* Script not found, error 404 */
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
			"%s", 404, "Page not found", "text/plain",
			"");

		fclose(skf);

		return 0;
	}

	pid = vfork();
	if (pid < 0) {
		HTTPD_ERROR("vfork() error");
		return pid;
	}

	if (pid == 0) {
		char *argv[] = { cmdname, NULL };
		char *envp[ARRAY_SIZE(cgi_env) + 1];
		char envbuf[128];
		char *ebp;
		size_t env_sz;
		int i_ce;

		ebp = envbuf;
		env_sz = sizeof(envbuf);
		for (i_ce = 0; i_ce < ARRAY_SIZE(cgi_env); i_ce++) {
			const struct cgi_env_descr *ce_d = &cgi_env[i_ce];
			int printed;

			printed = snprintf(ebp, env_sz, "%s=%s",
						ce_d->name,
						*(char **) ((void *) hreq + ce_d->hreq_offset));
			if (printed == env_sz) {
				HTTPD_ERROR("have no space to write environment");
				exit(1);
			}
			envp[i_ce] = ebp;

			ebp += printed + 1;
			env_sz -= printed + 1;

		}

		envp[ARRAY_SIZE(envp) - 1] = NULL;

		dup2(cinfo->ci_sock, STDIN_FILENO);
		dup2(cinfo->ci_sock, STDOUT_FILENO);
		dup2(cinfo->ci_sock, STDERR_FILENO);

#if 0
		execve(cmdname, argv, envp);
#else
		for (i_ce = 0; i_ce < ARRAY_SIZE(envp) - 1; i_ce++) {
			putenv(envp[i_ce]);
		}

		execv(cmdname, argv);
#endif

		exit(1);
	} else {
		while (pid != waitpid(pid, NULL, 0));
	}

	return 0;
}
#endif

static int httpd_read_http_header(const struct client_info *cinfo, char *buf, size_t buf_sz) {

        const int sk = cinfo->ci_sock;
	const char *pattern = "\r\n\r\n";
	char pattbuf[strlen("\r\n\r\n")];
	char *pb;

	pb = buf;
	if (0 > read(sk, pattbuf, sizeof(pattbuf))) {
		printf("[%02d] read 1 error\n", cinfo->id);
		return -errno;
	}
	while (0 != strncmp(pattern, pattbuf, sizeof(pattbuf)) && buf_sz > 0) {
		*(pb++) = pattbuf[0];
		buf_sz--;
		memmove(pattbuf, pattbuf + 1, sizeof(pattbuf) - 1);
		if (0 > read(sk, &pattbuf[sizeof(pattbuf) - 1], 1)) {
			printf("[%02d] read 2 error\n", cinfo->id);
			return -errno;
		}
	}

	if (buf_sz == 0) {
		printf("[%02d] read 3 error\n", cinfo->id);
		return -ENOENT;
	}

	memcpy(pb, pattbuf, sizeof(pattbuf));
	return pb + sizeof(pattbuf) - buf;
}

static int httpd_client_process(const struct client_info *cinfo) {
        struct http_req hreq;
	int ret;
        char httpd_inbuf[BUFF_SZ];

	ret = httpd_read_http_header(cinfo, httpd_inbuf, sizeof(httpd_inbuf));
	if (ret < 0) {
		HTTPD_ERROR("can't read from client socket: %s", strerror(errno));
		return ret;
        }
	httpd_inbuf[ret] = '\0';

	if (NULL == httpd_parse_request(httpd_inbuf, &hreq)) {
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

static void *do_httpd_client_thread(void *cinfo) {

        struct client_info *ci = (struct client_info *) cinfo;

        httpd_client_process(ci);
	close(ci->ci_sock);
        client_is_free[ci->ci_index]=1;
	return NULL;
}


int main(int argc, char **argv) {
	int host;
	int i;
	const char *basedir;
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

	basedir = argc > 1 ? argv[1] : "/";

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

	for (i = 0; i < MAX_CLIENTS_COUNT; ++i) {
		client_is_free[i] = 1;
        }
	while (1) {
		struct client_info *ci;
		pthread_t thread;
		int index = clients_get_free_index();

		if (index == -1) {
			HTTPD_ERROR("%s\n", "There are no more memory for new connection!");
			continue;
		}

		ci = &clients[index];
		ci->ci_index=index;
		ci->ci_addrlen = inaddrlen;
		ci->ci_sock = accept(host, &ci->ci_addr, &ci->ci_addrlen);
		if (ci->ci_sock == -1) {
			HTTPD_ERROR("accept() failure: %s", strerror(errno));
			continue;
		}
		assert(ci->ci_addrlen == inaddrlen);
		ci->ci_basedir = basedir;

		pthread_create(&thread, NULL, do_httpd_client_thread, ci);

		pthread_detach(thread);
	}

	close(host);

	return 0;
}

