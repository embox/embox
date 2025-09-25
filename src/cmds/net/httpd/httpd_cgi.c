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

#include <net/util/httpd.h>
#ifdef __EMBUILD_MOD__
#	include <framework/mod/options.h>
# define CGI_PREFIX  OPTION_STRING_GET(cgi_prefix)
#	define USE_REAL_CMD     OPTION_GET(BOOLEAN,use_real_cmd)
#	define USE_PARALLEL_CGI OPTION_GET(BOOLEAN,use_parallel_cgi)
#endif /* __EMBUILD_MOD__ */


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

/* TODO replace with execve */
static int httpd_execve(const char *path, char *const argv[], char *const envp[]) {
	for (int i_ce = 0; envp[i_ce]; i_ce++) {
		int res;
		res = putenv(envp[i_ce]);
		if (res) {
			httpd_error("putenv(%s) failed with %d", envp[i_ce], res);
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
		char *val = *(char **) ((char *) hreq + ce_d->hreq_offset);
		int printed;

		if (!val) {
			continue;
		}

		printed = snprintf(ebp, env_sz, "%s=%s", ce_d->name, val);
		if (env_sz <= printed) {
			httpd_error("have no space to write environment");
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
		httpd_error("vfork() error(%d): %s", err, strerror(err));
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

	httpd_on_cgi_child(cinfo, pid);
	return pid;
}

static int httpd_check_cgi(const struct http_req *hreq) {
	return 0 == strncmp(hreq->uri.target, CGI_PREFIX, strlen(CGI_PREFIX));
}


pid_t httpd_try_process(const struct client_info *cinfo, const struct http_req *hreq) {
	char path[HTTPD_MAX_PATH];
	struct stat fstat;
	int path_len;
	char *cmdname;

	if (!httpd_check_cgi(hreq)) {
		return 0;
	}

	path_len = snprintf(path, sizeof(path), "%s/%s", cinfo->ci_basedir, hreq->uri.target);
	if (path_len >= sizeof(path)) {
		return -ENOMEM;
	}

	if (0 == stat(path, &fstat)) {
		// file found in CGI_BIN folder
		return httpd_response_cgi(cinfo, hreq, path);
	}else if (USE_REAL_CMD){
		cmdname = hreq->uri.target + strlen(CGI_PREFIX);
		return httpd_response_cgi(cinfo, hreq, cmdname);
	}else{
		return 0;
	}
}
