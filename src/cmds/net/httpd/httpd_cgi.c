
#include <sys/types.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <util/array.h>

#include "httpd.h"

#include <framework/mod/options.h>
#define USE_REAL_CMD     OPTION_GET(BOOLEAN,use_real_cmd)
#define USE_PARALLEL_CGI OPTION_GET(BOOLEAN,use_parallel_cgi)

static char httpd_g_envbuf[256];
static const struct cgi_env_descr {
	char *name;
	off_t hreq_offset;
} cgi_env[] = {
	{ .name = "REQUEST_METHOD", .hreq_offset = offsetof(struct http_req, method) },
	{ .name = "CONTENT_LENGTH", .hreq_offset = offsetof(struct http_req, content_len) },
	{ .name = "CONTENT_TYPE", .hreq_offset = offsetof(struct http_req, content_type) },
	{ .name = "QUERY_STRING", .hreq_offset = offsetof(struct http_req, uri) + offsetof(struct http_req_uri, query) },
};

int httpd_send_response_cgi(const struct client_info *cinfo, const struct http_req *hreq) {
	char filename[HTTPD_MAX_PATH];
	char *cmdname;
	pid_t pid;
	struct stat fstat;

	snprintf(filename, sizeof(filename), "%s/%s", cinfo->ci_basedir, hreq->uri.target);
	filename[sizeof(filename) - 1] = '\0';
	cmdname = filename;

	if (stat(cmdname, &fstat)) {
#if ! USE_REAL_CMD
		FILE *skf;
		/* Script not found, error 404 */
		skf = fdopen(cinfo->ci_sock, "rw");
		if (!skf) {
			HTTPD_ERROR("can't allocate FILE for socket\n");
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
#else
		cmdname = hreq->uri.target + strlen(CGI_PREFIX);
#endif
	}

	pid = vfork();
	if (pid < 0) {
		HTTPD_ERROR("vfork() error\n");
		return pid;
	}

	if (pid == 0) {
		char *argv[] = { cmdname, NULL };
		char *envp[ARRAY_SIZE(cgi_env) + 1];
		char *ebp;
		size_t env_sz;
		int i_ce, n_ce;

		ebp = httpd_g_envbuf;
		env_sz = sizeof(httpd_g_envbuf);
		n_ce = 0;
		for (i_ce = 0; i_ce < ARRAY_SIZE(cgi_env); i_ce++) {
			const struct cgi_env_descr *ce_d = &cgi_env[i_ce];
			char *val = *(char **) ((void *) hreq + ce_d->hreq_offset);
			int printed;

			if (!val) {
				continue;
			}

			printed = snprintf(ebp, env_sz, "%s=%s", ce_d->name, val);
			if (printed == env_sz) {
				HTTPD_ERROR("have no space to write environment\n");
				exit(1);
			}
			envp[n_ce++] = ebp;

			ebp += printed + 1;
			env_sz -= printed + 1;
		}

		envp[n_ce] = NULL;

		dup2(cinfo->ci_sock, STDIN_FILENO);
		dup2(cinfo->ci_sock, STDOUT_FILENO);
		close(cinfo->ci_sock);

#if 0
		execve(cmdname, argv, envp);
#else
		for (i_ce = 0; envp[i_ce]; i_ce++) {
			if (putenv(envp[i_ce])) {
				HTTPD_ERROR("putenv failed\n");
				exit(1);
			}
		}

		execv(cmdname, argv);
#endif

		exit(1);
#if USE_PARALLEL_CGI
	} else {
		while (pid != waitpid(pid, NULL, 0));
#endif
	}

	return 0;
}

