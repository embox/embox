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

#ifdef __EMBUILD_MOD__
#include <framework/mod/options.h>
#define USE_REAL_CMD     OPTION_GET(BOOLEAN,use_real_cmd)
#define USE_PARALLEL_CGI OPTION_GET(BOOLEAN,use_parallel_cgi)
#endif

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

static char *httpd_exec_path_script(char *path) {
	struct stat fstat;
	if (stat(path, &fstat)) {
		return NULL;
	}
	return path;
}

static char *httpd_exec_path_cmd(char *path) {
	char *s = strstr(path, CGI_PREFIX);
	if (s) {
		return s + strlen(CGI_PREFIX);
	}
	return s;
}

static int httpd_error(const struct client_info *cinfo, int st, const char *msg) {
	FILE *skf = fdopen(cinfo->ci_sock, "rw");

	if (!skf) {
		HTTPD_ERROR("can't allocate FILE for socket\n");
		return -ENOMEM;
	}

	fprintf(skf,
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Connection: close\r\n"
		"\r\n"
		"%s", st, "", "text/plain",
		msg);

	fclose(skf);
	return 0;
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
		if (printed == env_sz) {
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

int httpd_send_response_cgi(const struct client_info *cinfo, const struct http_req *hreq) {
	char filename[HTTPD_MAX_PATH];
	char *cmdname;
	pid_t pid;

	snprintf(filename, sizeof(filename), "%s/%s", cinfo->ci_basedir, hreq->uri.target);
	filename[sizeof(filename) - 1] = '\0';

	cmdname = httpd_exec_path_script(filename);
	if (!cmdname && USE_REAL_CMD) {
		cmdname = httpd_exec_path_cmd(filename);
	} 
	if (!cmdname) {
		return httpd_error(cinfo, 404, "File not found");
	}

	pid = vfork();
	if (pid < 0) {
		HTTPD_ERROR("vfork() error\n");
		return pid;
	}

	if (pid == 0) {
		char *argv[] = { cmdname, NULL };
		char *envp[ARRAY_SIZE(cgi_env) + 1];

		httpd_fill_env(hreq, envp, ARRAY_SIZE(envp));

		dup2(cinfo->ci_sock, STDIN_FILENO);
		dup2(cinfo->ci_sock, STDOUT_FILENO);
		close(cinfo->ci_sock);

		httpd_execve(cmdname, argv, envp);
		exit(1);
	} else {
		if (!USE_PARALLEL_CGI) {
			while (pid != waitpid(pid, NULL, 0));
		}
	}

	return 0;
}
