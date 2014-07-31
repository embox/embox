/**
 * @file
 * @brief Parse http, https, ftp requests
 *
 * @date 24.10.12
 * @author Vita Loginova
 */

#include <net/util/request_parser.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <url_parser.h>

static const char * try_parse_method(http_request *parsed_request,
		const char *subrequest);
static const char * try_parse_url(http_request *parsed_request,
		const char *subrequest);
static const char * try_parse_proto(http_request *parsed_request,
		const char *subrequest);
static const char * try_parse_host(http_request *parsed_request,
		const char *subrequest);
static const char * try_parse_connection(http_request *parsed_request,
		const char *subrequest);

int request_parser_copy(http_request *to, const http_request *from) {
	assert(to != NULL);
	assert(from != NULL);

	memset(to, 0, sizeof(struct http_request));

	if (((from->method != NULL) && ((to->method = strdup(from->method)) == NULL))
			|| ((to->parsed_url = malloc(sizeof(struct parsed_url))) == NULL)
			|| ((to->parsed_url->scheme = strdup(from->parsed_url->scheme))
					== NULL)
			|| ((to->parsed_url->host = strdup(from->parsed_url->host)) == NULL)
			|| ((to->parsed_url->port = strdup(from->parsed_url->port)) == NULL)
			|| ((to->parsed_url->path = strdup(from->parsed_url->path)) == NULL)
			|| ((to->parsed_url->query = strdup(from->parsed_url->query))
					== NULL)
			|| ((to->parsed_url->fragment = strdup(from->parsed_url->fragment))
					== NULL)
			|| ((to->parsed_url->username = strdup(from->parsed_url->username))
					== NULL)
			|| ((to->parsed_url->password = strdup(from->parsed_url->password))
					== NULL)
			|| ((from->proto != NULL)
					&& ((to->proto = strdup(from->proto)) == NULL))
			|| ((from->connection != NULL)
					&& ((to->connection = strdup(from->connection)) == NULL))) {
		free_http_request(to);
		return -1;
	}

	return 0;
}

int parse_http(const char *request, http_request *parsed_request) {
	const char *subrequest;

	assert(request != NULL);
	assert(parsed_request != NULL);

	memset(parsed_request, 0, sizeof(struct http_request));

	if (((subrequest = try_parse_method(parsed_request, request)) == NULL)
			|| ((subrequest = try_parse_url(parsed_request, subrequest)) == NULL)
			|| ((subrequest = try_parse_proto(parsed_request, subrequest))
					== NULL)
			|| ((subrequest = try_parse_host(parsed_request, subrequest))
					== NULL)
			|| ((subrequest = try_parse_connection(parsed_request, subrequest))
					== NULL)) {
		free_http_request(parsed_request);
		return -1;
	}

	return 0;
}

void free_http_request(http_request *request) {
	assert(request != NULL);

	if (request->method != NULL) {
		free(request->method);
		request->method = NULL;
	}
	if (request->parsed_url != NULL) {
		parsed_url_free(request->parsed_url);
		request->parsed_url = NULL;
	}
	if (request->proto != NULL) {
		free(request->proto);
		request->proto = NULL;
	}
	if (request->connection != NULL) {
		free(request->connection);
		request->connection = NULL;
	}
}

static const char * try_parse_method(http_request *parsed_request,
		const char *subrequest) {
	const char *lexeme_end;
	size_t lexeme_length;

	assert(parsed_request != NULL);
	assert(subrequest != NULL);

	lexeme_end = strchr(subrequest, ' ');
	if (lexeme_end == NULL) {
		return NULL;
	}

	lexeme_length = lexeme_end - subrequest;
	if (lexeme_length == 0) {
		return NULL;
	}

	parsed_request->method = malloc(lexeme_length + 1);
	if (parsed_request->method == NULL) {
		return NULL;
	}

	strncpy(parsed_request->method, subrequest, lexeme_length);
	parsed_request->method[lexeme_length] = '\0';

	while (isupper(parsed_request->method[--lexeme_length])
			&& (lexeme_length != 0))
		;

	return lexeme_length == 0 ? lexeme_end + 1 : NULL;
}

static const char * try_parse_url(http_request *parsed_request,
		const char *subrequest) {
	const char *lexeme_end;
	char *url;
	size_t lexeme_length;

	assert(parsed_request != NULL);
	assert(subrequest != NULL);

	lexeme_end = strchr(subrequest, ' ');
	if (lexeme_end == NULL) {
		return NULL;
	}

	lexeme_length = lexeme_end - subrequest;
	if (lexeme_length == 0) {
		return NULL;
	}

	url = malloc(lexeme_length + 1);
	if (url == NULL) {
		return NULL;
	}

	strncpy(url, subrequest, lexeme_length);
	url[lexeme_length] = '\0';

	parsed_request->parsed_url = parse_url(url);

	free(url);

	return parsed_request->parsed_url != NULL ? lexeme_end + 1 : NULL;
}

static const char * try_parse_proto(http_request *parsed_request,
		const char *subrequest) {
	char *lexeme_end;
	size_t lexeme_length;

	assert(parsed_request != NULL);
	assert(subrequest != NULL);

	lexeme_end = strchr(subrequest, '\r');
	lexeme_end = lexeme_end != NULL ? lexeme_end : strchr(subrequest, '\0');
	assert(lexeme_end != NULL);

	lexeme_length = lexeme_end - subrequest;
	if (lexeme_length == 0) {
		return NULL;
	}

	parsed_request->proto = malloc(lexeme_length + 1);
	if (parsed_request->proto == NULL) {
		return NULL;
	}

	strncpy(parsed_request->proto, subrequest, lexeme_length);
	parsed_request->proto[lexeme_length] = '\0';

	while ((isupper(parsed_request->proto[--lexeme_length])
			|| isdigit(parsed_request->proto[lexeme_length])
			|| (parsed_request->proto[lexeme_length] == '.')
			|| (parsed_request->proto[lexeme_length] == '/'))
			&& (lexeme_length != 0))
		;

	if (lexeme_end[0] != '\0') {
		lexeme_end++;
	}
	return lexeme_length == 0 ? lexeme_end : NULL;
}

static const char * try_parse_host(http_request *parsed_request,
		const char *subrequest) {
	const char *lexeme_end, expected_host_identifyer[] = "Host:";
	char *url;
	size_t lexeme_length;
	struct parsed_url *parsed_host;

	assert(parsed_request != NULL);
	assert(subrequest != NULL);

	if ((parsed_request->parsed_url->host != NULL)
			&& (parsed_request->parsed_url->port != NULL)) {
		return NULL;
	}

	if (subrequest[0] == '\0') {
		return subrequest;
	} else if (subrequest[0] != '\n') {
		return NULL;
	}

	lexeme_end = strchr(++subrequest, ' ');
	if (lexeme_end == NULL) {
		return NULL;
	}

	lexeme_length = lexeme_end - subrequest;
	if (lexeme_length == 0) {
		return NULL;
	}

	if ((lexeme_length != sizeof expected_host_identifyer - 1)
			|| (strncmp(subrequest, &expected_host_identifyer[0], lexeme_length)
					!= 0)) {
		return subrequest;
	}

	subrequest = ++lexeme_end;

	lexeme_end = strchr(subrequest, '\r');
	lexeme_end = lexeme_end != NULL ? lexeme_end : strchr(subrequest, '\0');
	assert(lexeme_end != NULL);

	lexeme_length = lexeme_end - subrequest;
	if (lexeme_length == 0) {
		return NULL;
	}

	url = malloc(lexeme_length + 1);
	if (url == NULL) {
		return NULL;
	}

	strncpy(url, subrequest, lexeme_length);
	url[lexeme_length] = '\0';

	parsed_host = parse_url(url);

	free(url);

	if (parsed_host == NULL) {
		return NULL;
	}

	if ((parsed_request->parsed_url->host == NULL)
			&& (parsed_host->host != NULL)) {
		lexeme_length = strlen(parsed_host->host);

		parsed_request->parsed_url->host = malloc(lexeme_length + 1);
		if (parsed_request->parsed_url->host != NULL) {
			memcpy(parsed_request->parsed_url->host, parsed_host->host,
					lexeme_length + 1);
		}
	}

	if ((parsed_request->parsed_url->port == NULL)
			&& (parsed_host->port != NULL)) {
		lexeme_length = strlen(parsed_host->port);

		parsed_request->parsed_url->port = malloc(lexeme_length + 1);
		if (parsed_request->parsed_url->port != NULL) {
			memcpy(parsed_request->parsed_url->port, parsed_host->port,
					lexeme_length + 1);
		}
	}

	parsed_url_free(parsed_host);

	return lexeme_end + 1;
}

static const char * try_parse_connection(http_request *parsed_request,
		const char *subrequest) {
	const char *lexeme_end, expected_connection_identifyer[] = "Connection:";
	size_t lexeme_length;

	if (subrequest[0] == '\0') {
		return subrequest;
	} else if (subrequest[0] != '\n') {
		return NULL;
	}

	lexeme_end = strchr(++subrequest, ' ');
	if (lexeme_end == NULL) {
		return NULL;
	}

	lexeme_length = lexeme_end - subrequest;
	if (lexeme_length == 0) {
		return NULL;
	}

	if ((lexeme_length != sizeof expected_connection_identifyer - 1)
			|| (strncmp(subrequest, &expected_connection_identifyer[0],
					lexeme_length) != 0)) {
		return subrequest;
	}

	subrequest = ++lexeme_end;

	lexeme_end = strchr(subrequest, '\r');
	lexeme_end = lexeme_end != NULL ? lexeme_end : strchr(subrequest, '\0');
	assert(lexeme_end != NULL);

	lexeme_length = lexeme_end - subrequest;
	if (lexeme_length == 0) {
		return NULL;
	}

	parsed_request->connection = malloc(lexeme_length + 1);
	if (parsed_request->connection == NULL) {
		return NULL;
	}

	strncpy(parsed_request->connection, subrequest, lexeme_length);
	parsed_request->connection[lexeme_length] = '\0';

	return lexeme_end + 1;
}
