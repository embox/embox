/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.04.2015
 */

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "httpd.h"

static char httpd_g_envbuf[256];

struct cgi_env_descr {
	char *name;
	off_t hreq_offset;
};

static const struct cgi_env_descr cgi_env[] = {
	{ .name = "REQUEST_METHOD", .hreq_offset = offsetof(struct http_req, method) },
	{ .name = "CONTENT_LENGTH", .hreq_offset = offsetof(struct http_req, content_len) },
	{ .name = "CONTENT_TYPE", .hreq_offset = offsetof(struct http_req, content_type) },
	{ .name = "QUERY_STRING", .hreq_offset = offsetof(struct http_req, uri) + offsetof(struct http_req_uri, query) },
};

/* TODO replace with execve */
static int httpd_execve(const char *path, char *const argv[], char *const envp[]) {
	int i_ce;
	for (i_ce = 0; envp[i_ce]; i_ce++) {
		if (putenv(envp[i_ce])) {
			HTTPD_ERROR("putenv failed\n");
			exit(1);
		}
	}

	return execv(path, argv);
}

static int httpd_fill_env(const struct http_req *hreq, char *envp[], int envp_len) {
	char *ebp;
	size_t env_sz;
	int i_ce, n_ce;

	ebp = httpd_g_envbuf;
	env_sz = sizeof(httpd_g_envbuf);
	n_ce = 0;

	assert(envp_len >= ARRAY_SIZE(cgi_env));

	for (i_ce = 0; i_ce < ARRAY_SIZE(cgi_env); i_ce++) {
		const struct cgi_env_descr *ce_d = &cgi_env[i_ce];
		char *val = *(char **) ((void *) hreq + ce_d->hreq_offset);
		int printed;

		if (!val) {
			continue;
		}

		printed = snprintf(ebp, env_sz, "%s=%s", ce_d->name, val);
		if (env_sz <= printed) {
			HTTPD_ERROR("have no space to write environment\n");
			exit(1);
		}
		envp[n_ce++] = ebp;

		ebp += printed + 1;
		env_sz -= printed + 1;
	}

	envp[n_ce] = NULL;

	return n_ce;
}

static pid_t httpd_response_cgi(const struct client_info *cinfo, const struct http_req *hreq, 
		char *path) {
	pid_t pid;

	pid = vfork();
	if (pid < 0) {
		int err = errno;
		HTTPD_ERROR("vfork() error(%d): %s\n", err, strerror(err));
		return -err;
	}

	if (pid == 0) {
		char *argv[] = { path, NULL };
		char *envp[ARRAY_SIZE(cgi_env) + 1];

		httpd_fill_env(hreq, envp, ARRAY_SIZE(envp));

		dup2(cinfo->ci_sock, STDIN_FILENO);
		dup2(cinfo->ci_sock, STDOUT_FILENO);
		close(cinfo->ci_sock);

		httpd_execve(path, argv, envp);
		exit(1);
	}

	return pid;
}

static int httpd_check_cgi(const struct http_req *hreq) {
	return 0 == strncmp(hreq->uri.target, CGI_PREFIX, strlen(CGI_PREFIX));
}

pid_t httpd_try_respond_script(const struct client_info *cinfo, const struct http_req *hreq) {
	char filename[HTTPD_MAX_PATH];
	struct stat fstat;

	if (!httpd_check_cgi(hreq)) {
		return 0;
	}

	if (sizeof(filename) <= snprintf(filename, sizeof(filename), 
				"%s/%s", cinfo->ci_basedir, hreq->uri.target)) {
		return -ERANGE;
	}

	if (0 != stat(filename, &fstat)) {
		return 0;
	}

	return httpd_response_cgi(cinfo, hreq, filename);
}

pid_t httpd_try_respond_cmd(const struct client_info *cinfo, const struct http_req *hreq) {
	char *cmdname;

	if (!httpd_check_cgi(hreq)) {
		return 0;
	}

	cmdname = hreq->uri.target + strlen(CGI_PREFIX);
	return httpd_response_cgi(cinfo, hreq, cmdname);
}
