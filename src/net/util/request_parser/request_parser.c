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




