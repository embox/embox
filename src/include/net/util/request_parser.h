/**
 * @file
 * @brief
 *
 * @date 25.10.12
 * @author Vita Loginova
 */

#ifndef NET_UTIL_REQUEST_PARSER_H_
#define NET_UTIL_REQUEST_PARSER_H_

struct parsed_url; /* from url_parser.h */

struct http_request {
	char *method;
	struct parsed_url *parsed_url;
	char *proto;
	char *connection;
};

typedef struct http_request http_request;

/**
 * Check and parse http request line
 *
 * @param request - http request string
 * @param http_request blunk struct http_request
 * @retval -1 on fail 0 on success
 */
extern int parse_http(const char *request, http_request *parsed_request);

/**
 * free http_request memory
 *
 * @param request - http_request to free
 */
extern void free_http_request(http_request *request);

extern int request_parser_copy(http_request *to, const http_request *from);

#endif /* NET_UTIL_REQUEST_PARSER_H_ */
