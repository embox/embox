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

struct http_request{
	char * method;
};

typedef struct http_request http_request;

/*Parse HTTP, now two methods only. Returns 1 if there is matches*/
extern int parse_http(char * request, struct http_request * parsed_request);

#endif /* REQUEST_PARSER_H_ */
