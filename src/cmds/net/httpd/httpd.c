/**
 * @file
 * @brief Simple HTTP server
 * @date 16.04.12
 * @author Ilia Vaprol
 * @author Anton Kozlov
 * 	- CGI related changes
 * @author Andrey Golikov
 * 	- Linux adaptation
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include <netinet/in.h>

#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <math.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

#include <ifaddrs.h>

#include "httpd.h"

#define ARRAY_SIZE(array) \
	(sizeof(array) / sizeof(*(array)))

#ifdef __EMBUILD_MOD__
	#include <net/l3/ipv4/ip.h>
	#include <net/inetdevice.h>
	#include <embox/cmd.h>
	#include <util/math.h>
	#include <framework/mod/options.h>

	#define USE_IP_VER       OPTION_GET(NUMBER,use_ip_ver)
	#define USE_CGI          OPTION_GET(BOOLEAN,use_cgi)
#endif

#define BUFF_SZ     1024
#define PAGE_INDEX  "index.html"

static char httpd_g_inbuf[BUFF_SZ];

static int httpd_read_http_header(const struct client_info *cinfo, char *buf, size_t buf_sz) {
	const int sk = cinfo->ci_sock;
	const char *pattern = "\r\n\r\n";
	char pattbuf[strlen("\r\n\r\n")];
	char *pb;

	pb = buf;
	if (0 > read(sk, pattbuf, sizeof(pattbuf))) {
		return -errno;
	}
	while (0 != strncmp(pattern, pattbuf, sizeof(pattbuf)) && buf_sz > 0) {
		*(pb++) = pattbuf[0];
		buf_sz--;
		memmove(pattbuf, pattbuf + 1, sizeof(pattbuf) - 1);
		if (0 > read(sk, &pattbuf[sizeof(pattbuf) - 1], 1)) {
			return -errno;
		}
	}

	if (buf_sz == 0) {
		return -ENOENT;
	}

	memcpy(pb, pattbuf, sizeof(pattbuf));
	return pb + sizeof(pattbuf) - buf;
}

static int httpd_client_process(const struct client_info *cinfo) {
	struct http_req hreq;
	int ret;

	ret = httpd_read_http_header(cinfo, httpd_g_inbuf, sizeof(httpd_g_inbuf) - 1);
	if (ret < 0) {
		HTTPD_ERROR("can't read from client socket: %s\n", strerror(errno));
		return ret;
	}
	httpd_g_inbuf[ret] = '\0';

	memset(&hreq, 0, sizeof(hreq));
	if (NULL == httpd_parse_request(httpd_g_inbuf, &hreq)) {
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

static int httpd_collect_cgi_childs(void) {
	pid_t child;

	do {
		child = waitpid(-1, NULL, WNOHANG);
		if ((child == -1) && (errno != EINTR)) {
			HTTPD_ERROR("waitpid() : %s\n", strerror(errno));
			break;
		}
	} while (child != 0);

	return child;
}

int main(int argc, char **argv) {
	int host;
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
		HTTPD_ERROR("socket() failure: %s\n", strerror(errno));
		return -errno;
	}

	if (-1 == bind(host, (struct sockaddr *) &inaddr, inaddrlen)) {
		HTTPD_ERROR("bind() failure: %s\n", strerror(errno));
		close(host);
		return -errno;
	}

	if (-1 == listen(host, 3)) {
		HTTPD_ERROR("listen() failure: %s\n", strerror(errno));
		close(host);
		return -errno;
	}

	while (1) {
		struct client_info ci;

		ci.ci_addrlen = inaddrlen;
		ci.ci_sock = accept(host, &ci.ci_addr, &ci.ci_addrlen);
		if (ci.ci_sock == -1) {
			if (errno != EINTR) {
				HTTPD_ERROR("accept() failure: %s\n", strerror(errno));
				usleep(100000);
			}
			continue;
		}
		assert(ci.ci_addrlen == inaddrlen);
		ci.ci_basedir = basedir;

		httpd_collect_cgi_childs();

		httpd_client_process(&ci);

		close(ci.ci_sock);
	}

	close(host);

	return 0;
}
