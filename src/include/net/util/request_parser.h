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
 * @retval new http_request on success match
 * @retval NULL on fail
 */
extern http_request * parse_http(char * request);

/**
 * free http_request memory
 *
 * @param request - http_request to free
 */
extern void free_http_request(http_request *request);

#endif /* REQUEST_PARSER_H_ */
