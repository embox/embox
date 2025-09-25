/**
 * @file
 * @brief Simple HTTP server
 * @date 08.07.2015
 * @author Ilia Vaprol
 * @author Anton Kozlov
 * 	- CGI related changes
 * @author Andrey Golikov
 * 	- Linux adaptation
 * @author Anastasia Vinogradova
 * 	- multithread support
 */

#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <net/util/httpd.h>

#ifdef __EMBUILD_MOD__
#	include <framework/mod/options.h>
#	define USE_IP_VER       OPTION_GET(NUMBER,use_ip_ver)
#endif /* __EMBUILD_MOD__ */

#define BUFF_SZ     1024
#define MAX_CLIENTS_COUNT 4

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

static void httpd_client_process(struct client_info *cinfo) {
	struct http_req hreq;
	int err;
	char httpd_inbuf[BUFF_SZ];
	char httpd_outbuf[BUFF_SZ];

	if (0 > (err = httpd_build_request(cinfo, &hreq, httpd_inbuf, sizeof(httpd_inbuf)))) {
		httpd_error("can't build request: %s", strerror(-err));
	}

	httpd_debug("method=%s uri_target=%s uri_query=%s",
			hreq.method, hreq.uri.target, hreq.uri.query);

	if (httpd_try_respond_file(cinfo, &hreq,
				httpd_outbuf, sizeof(httpd_outbuf))) {
		/* file sent, nothing to do */
	} else {
		httpd_header(cinfo, 404, "");
	}
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
#elif USE_IP_VER == 6
	struct sockaddr_in6 inaddr;
	const size_t inaddrlen = sizeof(inaddr);
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

	if (-1 == bind(host, (struct sockaddr *) &inaddr, inaddrlen)) {
		httpd_error("bind() failure: %s", strerror(errno));
		close(host);
		return -errno;
	}

	if (-1 == listen(host, 3)) {
		httpd_error("listen() failure: %s", strerror(errno));
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
			httpd_error("There are no more memory for new connection!");
			continue;
		}

		ci = &clients[index];
		ci->ci_index=index;
		ci->ci_addrlen = inaddrlen;
		ci->ci_sock = accept(host, &ci->ci_addr, &ci->ci_addrlen);
		if (ci->ci_sock == -1) {
			httpd_error("accept() failure: %s", strerror(errno));
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

