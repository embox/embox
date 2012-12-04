/**
 * @file
 * @brief Parse http, https, ftp requests
 *
 * @date 24.10.12
 * @author Vita Loginova
 */

#include <net/util/request_parser.h>
#include <lib/url_parser.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char * try_parse_method(http_request *parsed_request, char *subrequenst);
char * try_parse_url(http_request *parsed_request, char *subrequenst);
char * try_parse_proto(http_request *parsed_request, char *subrequenst);
char * try_parse_host(http_request *parsed_request, char *subrequenst);

void request_parser_full_strcpy(char *to, char *from) {
	strcpy(to, from);
	to[strlen(from)] = 0;
}

void request_parser_cpy(http_request *to, http_request *from) {
	to->method = malloc(strlen(from->method) + 1);
	request_parser_full_strcpy(to->method, from->method);

	to->parsed_url->fragment = malloc(strlen(from->parsed_url->fragment) + 1);
	request_parser_full_strcpy(to->parsed_url->fragment,
			from->parsed_url->fragment);

	to->parsed_url->host = malloc(strlen(from->parsed_url->host) + 1);
	request_parser_full_strcpy(to->parsed_url->host, from->parsed_url->host);

	to->parsed_url->password = malloc(strlen(from->parsed_url->password) + 1);
	request_parser_full_strcpy(to->parsed_url->password,
			from->parsed_url->password);

	to->parsed_url->path = malloc(strlen(from->parsed_url->path) + 1);
	request_parser_full_strcpy(to->parsed_url->path, from->parsed_url->path);

	to->parsed_url->port = malloc(strlen(from->parsed_url->port) + 1);
	request_parser_full_strcpy(to->parsed_url->port, from->parsed_url->port);

	to->parsed_url->query = malloc(strlen(from->parsed_url->query) + 1);
	request_parser_full_strcpy(to->parsed_url->query, from->parsed_url->query);

	to->parsed_url->scheme = malloc(strlen(from->parsed_url->scheme) + 1);
	request_parser_full_strcpy(to->parsed_url->scheme,
			from->parsed_url->scheme);

	to->parsed_url->username = malloc(strlen(from->parsed_url->username) + 1);
	request_parser_full_strcpy(to->parsed_url->username,
			from->parsed_url->username);

	to->proto = malloc(strlen(from->proto) + 1);
	request_parser_full_strcpy(to->proto, from->proto);

}

http_request *parse_http(char * request) {
	http_request *parsed_request;
	char * subrequenst;

	parsed_request = malloc(sizeof(http_request));
	parsed_request->method = NULL;
	parsed_request->parsed_url = NULL;
	parsed_request->proto = NULL;

	subrequenst = request;

	subrequenst = try_parse_method(parsed_request, subrequenst);
	if (subrequenst == NULL) {
		free_http_request(parsed_request);
		return NULL;
	}

	subrequenst = try_parse_url(parsed_request, subrequenst);
	if (subrequenst == NULL) {
		free_http_request(parsed_request);
		return NULL;
	}

	subrequenst = try_parse_proto(parsed_request, subrequenst);
	if (subrequenst == NULL) {
		free_http_request(parsed_request);
		return NULL;
	}

	subrequenst = try_parse_host(parsed_request, subrequenst);
	if (subrequenst == NULL) {
		free_http_request(parsed_request);
		return NULL;
	}

	return parsed_request;
}

void free_http_request(http_request *request) {
	if (NULL != request) {
		if (NULL != request->method) {
			free(request->method);
		}
		if (NULL != request->proto) {
			free(request->proto);
		}
		if (NULL != request->parsed_url) {
			parsed_url_free(request->parsed_url);
		}
		free(request);
	}
}

char *try_parse_method(http_request *parsed_request, char *subrequenst) {
	int lexeme_length;
	char * lexeme_end;
	lexeme_end = strchr(subrequenst, ' ');

	if (lexeme_end != NULL) {
		lexeme_length = lexeme_end - subrequenst;

		if (lexeme_length <= 0) {
			return NULL;
		}

		parsed_request->method = malloc(lexeme_length + 1);
		if (NULL == parsed_request->method) {
			return NULL;
		}
		(void) strncpy(parsed_request->method, subrequenst, lexeme_length);
		parsed_request->method[lexeme_length] = '\0';
		for (int i = 0; i < lexeme_length; i++) {
			if (!isupper(parsed_request->method[i])) {
				return NULL;
			}
		}
		lexeme_end++;
		return lexeme_end;
	}
	return NULL;
}

char *try_parse_url(http_request *parsed_request, char *subrequenst) {
	int lexeme_length;
	char * lexeme_end;
	lexeme_end = strchr(subrequenst, ' ');
	if (lexeme_end != NULL) {
		char * url;
		lexeme_length = lexeme_end - subrequenst;

		if (lexeme_length <= 0) {
			return NULL;
		}

		url = malloc(lexeme_length + 1);
		if (NULL == url) {
			return NULL;
		}
		strncpy(url, subrequenst, lexeme_length);
		url[lexeme_length] = '\0';
		parsed_request->parsed_url = parse_url(url);
		free(url);
		if (NULL == parsed_request->parsed_url) {
			return NULL;
		}
		lexeme_end++;
		return lexeme_end;
	}
	return NULL;
}

char *try_parse_proto(http_request *parsed_request, char *subrequenst) {
	int lexeme_length;
	char * lexeme_end;

	lexeme_end =
			strchr(subrequenst, '\r') != NULL ?
					strchr(subrequenst, '\r') : strchr(subrequenst, '\0');
	lexeme_length = lexeme_end - subrequenst;

	if (lexeme_length <= 0) {
		return NULL;
	}

	parsed_request->proto = malloc(lexeme_length + 1);

	if (NULL == parsed_request->proto) {
		return NULL;
	}
	(void) strncpy(parsed_request->proto, subrequenst, lexeme_length);
	parsed_request->proto[lexeme_length] = '\0';
	for (int i = 0; i < lexeme_length; i++) {
		if ((!isupper(parsed_request->proto[i]))
				&& (!isdigit(parsed_request->proto[i]))
				&& (parsed_request->proto[i] != '.')
				&& (parsed_request->proto[i] != '/')) {
			return NULL;
		}
	}
	lexeme_end++;
	return lexeme_end;
}

char *try_parse_host(http_request *parsed_request, char *subrequenst) {
	int lexeme_length;
	char * lexeme_end;
	char * host_identifyer;
	struct parsed_url * parsed_host;
	char *expected_host_identifyer = "Host:";

	if (strlen(subrequenst) == 0) {
		return subrequenst;
	}

	if (subrequenst[0] == '\n') {
		subrequenst++;
	} else {
		return NULL;
	}

	lexeme_end = strchr(subrequenst, ' ');
	lexeme_length = lexeme_end - subrequenst;

	if (lexeme_length <= 0) {
		return NULL;
	}

	host_identifyer = malloc(lexeme_length + 1);

	(void) strncpy(host_identifyer, subrequenst, lexeme_length);
	host_identifyer[lexeme_length] = '\0';

	if ((strlen(expected_host_identifyer) != lexeme_length)
			|| (strncmp(host_identifyer, expected_host_identifyer,
					lexeme_length) != 0)) {
		free(host_identifyer);
		return subrequenst;
	}
	if (parsed_request->parsed_url->host != NULL) {
		return NULL;
	}
	free(host_identifyer);
	subrequenst = ++lexeme_end;
	lexeme_end =
			strchr(subrequenst, '\r') != NULL ?
					strchr(subrequenst, '\r') : strchr(subrequenst, '\0');
	if (lexeme_end != NULL) {
		char * url;
		lexeme_length = lexeme_end - subrequenst;
		url = malloc(lexeme_length + 1);
		if (NULL == url) {
			return NULL;
		}
		(void) strncpy(url, subrequenst, lexeme_length);
		url[lexeme_length] = '\0';
		parsed_host = parse_url(url);
		free(url);
		if ((NULL == parsed_host) || (parsed_host->host == NULL)) {
			return NULL;
		}
		lexeme_length = strlen(parsed_host->host);
		parsed_request->parsed_url->host = malloc(lexeme_length + 1);
		strncpy(parsed_request->parsed_url->host, parsed_host->host,
				lexeme_length);
		parsed_request->parsed_url->host[lexeme_length] = '\0';
		if (parsed_host->port != NULL) {
			lexeme_length = strlen(parsed_host->port);
			parsed_request->parsed_url->port = malloc(lexeme_length + 1);
			strncpy(parsed_request->parsed_url->port, parsed_host->port,
					lexeme_length);
			parsed_request->parsed_url->port[lexeme_length] = '\0';
		}
		lexeme_end++;
		parsed_url_free(parsed_host);
		return lexeme_end;
	}
	return NULL;
}

