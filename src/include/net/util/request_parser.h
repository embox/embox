/**
 * @file
 *
 * @brief
 *
 * @date 25.10.2012
 * @author Vita Loginova
 */

#ifndef REQUEST_PARSER_H_
#define REQUEST_PARSER_H_

struct http_request {
	char *method;
	struct parsed_url *parsed_url;
	char *proto;
};

typedef struct http_request http_request;

/**
 * Check and parse http request line
 *
 * @param request - http request string
 * @param http_request blunk struct http_request
 * @retval -1 on fail 0 on success
 */
extern int parse_http(char * request, http_request *parsed_request);

/**
 * free http_request memory
 *
 * @param request - http_request to free
 */
extern void free_http_request(http_request *request);

extern void request_parser_cpy(http_request *to, http_request *from);

#endif /* REQUEST_PARSER_H_ */
