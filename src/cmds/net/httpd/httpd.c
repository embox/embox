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

#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include "httpd.h"

#ifdef __EMBUILD_MOD__
#	include <framework/mod/options.h>
#	define USE_IP_VER       OPTION_GET(NUMBER,use_ip_ver)
#	define USE_CGI          OPTION_GET(BOOLEAN,use_cgi)
#	define USE_REAL_CMD     OPTION_GET(BOOLEAN,use_real_cmd)
#	define USE_PARALLEL_CGI OPTION_GET(BOOLEAN,use_parallel_cgi)
#endif /* __EMBUILD_MOD__ */

#define BUFF_SZ     1024

static char httpd_g_inbuf[BUFF_SZ];
static char httpd_g_outbuf[BUFF_SZ];

static int httpd_wait_cgi_child(pid_t target, int opts) {
	pid_t child;

	do {
		child = waitpid(target, NULL, opts);
	} while (child == -1 && errno == EINTR);

	if (child == -1) {
		int err = errno;
		httpd_error("waitpid() : %s", strerror(err));
		return -err;
	}

	return child;
}

static void httpd_on_cgi_child(const struct client_info *cinfo, pid_t child) {
	if (child > 0) {
	       if (!USE_PARALLEL_CGI) {
		       httpd_wait_cgi_child(child, 0);
	       }
	} else {
		httpd_header(cinfo, 500, strerror(-child));
	}
}

static void httpd_client_process(struct client_info *cinfo) {
	struct http_req hreq;
	pid_t cgi_child;
	int err;

	if (0 > (err = httpd_build_request(cinfo, &hreq, httpd_g_inbuf, sizeof(httpd_g_inbuf)))) {
		httpd_error("can't build request: %s", strerror(-err));
	}

	httpd_debug("method=%s uri_target=%s uri_query=%s",
			   hreq.method, hreq.uri.target, hreq.uri.query);

	if ((cgi_child = httpd_try_respond_script(cinfo, &hreq))) {
		httpd_on_cgi_child(cinfo, cgi_child);
	} else if (USE_REAL_CMD && (cgi_child = httpd_try_respond_cmd(cinfo, &hreq))) {
		httpd_on_cgi_child(cinfo, cgi_child);
	} else if (httpd_try_respond_file(cinfo, &hreq,
				httpd_g_outbuf, sizeof(httpd_g_outbuf))) {
		/* file sent, nothing to do */
	} else {
		httpd_header(cinfo, 404, "");
	}
}

int main(int argc, char **argv) {
	int host;
	const char *basedir;
#if USE_IP_VER == 4
	struct sockaddr_in inaddr;
	const int family = AF_INET;

	inaddr.sin_family = AF_INET;
	inaddr.sin_port= htons(80);
	inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
#elif USE_IP_VER == 6
	struct sockaddr_in6 inaddr;
	const int family = AF_INET6;

	inaddr.sin6_family = AF_INET6;
	inaddr.sin6_port= htons(80);
	memcpy(&inaddr.sin6_addr, &in6addr_any, sizeof(inaddr.sin6_addr));
#else
#error Unknown USE_IP_VER
#endif

	basedir = argc > 1 ? argv[1] : "/";

	host = socket(family, SOCK_STREAM, IPPROTO_TCP);
	if (host == -1) {
		httpd_error("socket() failure: %s", strerror(errno));
		return -errno;
	}

	if (-1 == bind(host, (struct sockaddr *) &inaddr, sizeof(inaddr))) {
		httpd_error("bind() failure: %s", strerror(errno));
		close(host);
		return -errno;
	}

	if (-1 == listen(host, 3)) {
		httpd_error("listen() failure: %s", strerror(errno));
		close(host);
		return -errno;
	}

	while (1) {
		struct client_info ci;

		ci.ci_addrlen = sizeof(inaddr);
		ci.ci_sock = accept(host, &ci.ci_addr, &ci.ci_addrlen);
		if (ci.ci_sock == -1) {
			if (errno != EINTR) {
				httpd_error("accept() failure: %s", strerror(errno));
				usleep(100000);
			}
			continue;
		}
		assert(ci.ci_addrlen == sizeof(inaddr));
		ci.ci_basedir = basedir;

		if (USE_PARALLEL_CGI) {
			while (0 < httpd_wait_cgi_child(-1, WNOHANG)) {
				/* wait another one */
			}
		}

		httpd_client_process(&ci);

		close(ci.ci_sock);
	}

	close(host);

	return 0;
}
