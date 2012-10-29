/**
 * @file
 * @brief Parse http, https, ftp requests
 *
 * @date 24.10.12
 * @author Vita Loginova
 */

#include <net/util/request_parser.h>
#include <lib/trex.h>
#include <stdio.h>
#include <string.h>

int parse_http(char * request, struct http_request * parsed_request) {

	const TRexChar *error = NULL;
	TRex *trex = trex_compile(_TREXC("^((GET)|(HEAD))"), &error);
	if (trex) {
		const TRexChar *begin, *end;
		if (trex_search(trex, request, &begin, &end)) {
			int n = trex_getsubexpcount(trex);
			int i;
			TRexMatch match;
			for (i = 0; i < n; i++) {
				trex_getsubexp(trex, i, &match);
				if (match.len > 0)
				{
					strncpy(parsed_request->method, match.begin, match.len);
				}
			}

		} else {
			return 0;
		}
		trex_free(trex);
		return 1;
	}
	return 0;
}

int check_firstline (char * request){

	char *command_pattern = "(GET|HEAD)";
	char *domain_pattern = "(([\\w]([-\\w]*[\\w])*\\.)+[\\a]{2,})";
	char *ip4_pattern = "((\\d{1,3}\\.){3}\\d{1,3})";
	char *port_pattern = "(:\\d+)";
	char *path_pattern = "(/[-\\w_.~+]*)*";
	char *query_string = "(\\?[-\\w%%_.,~+=&;]*)";
	char *fragment_pattern = "(#[-\\w_]*)";
	char *proto_pattern = "(HTTP/1\\.1)";
	const TRexChar *error = NULL;
	TRex *trex;
	char pattern[256];

	sprintf(pattern, "^%s\\s((%s|%s)%s?)?%s%s?%s?\\s%s$", command_pattern, domain_pattern,
			ip4_pattern, port_pattern, path_pattern, query_string, fragment_pattern, proto_pattern);
	trex = trex_compile(_TREXC(pattern), &error);
		if (trex) {
			const TRexChar *begin, *end;
			if (trex_search(trex, request, &begin, &end)) {
				/*TODO extract groups*/
//				int n = trex_getsubexpcount(trex);
//				int i;
//				TRexMatch match;
//				for (i = 0; i < n; i++) {
//					trex_getsubexp(trex, i, &match);
//					if (match.len > 0)
//					{
//						char check[200] = "";
//						strncpy(check, match.begin, match.len);
//						printf("%s\n", check);
//					}
//				}
			} else {
				return 0;
			}
			trex_free(trex);
			return 1;
		}
		return 0;
}



