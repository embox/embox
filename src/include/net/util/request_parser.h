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

/*check first line; methods: GET and HEAD; url - domain or ip4; proto HTTP/1.1*/
extern int check_firstline (char * request);

#endif /* REQUEST_PARSER_H_ */
