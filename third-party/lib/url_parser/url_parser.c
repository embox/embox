/*
 * Copyright 2010 Scyphus Solutions Co. Ltd.  All rights reserved.
 *
 * Authors:
 *      Hirochika Asai
 *
 *  Modified on: Oct 31, 2012
 *      Author: vita
 */

#include "url_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Prototype declarations
 */
static __inline__ int _is_scheme_char(int);

/*
 * Check whether the character is permitted in scheme string
 */
static __inline__ int _is_scheme_char(int c) {
	return (!isalpha(c) && '+' != c && '-' != c && '.' != c) ? 0 : 1;
}

/*
 * See RFC 1738, 3986
 */
struct parsed_url *parse_url(const char *url) {
	//todo check all elements for correctness
	struct parsed_url *purl;
	const char *tmpstr;
	const char *curstr;
	int len;
	int i;
	int userpass_flag;
	int bracket_flag;

	/* Allocate the parsed url storage */
	purl = malloc(sizeof(struct parsed_url));
	if (NULL == purl) {
		return NULL;
	}
	purl->scheme = NULL;
	purl->host = NULL;
	purl->port = NULL;
	purl->path = NULL;
	purl->query = NULL;
	purl->fragment = NULL;
	purl->username = NULL;
	purl->password = NULL;

	curstr = url;

	/*
	 * <scheme>:<scheme-specific-part>
	 * <scheme> := [a-z\+\-\.]+
	 *             upper case = lower case for resiliency
	 */
	/* Read scheme */
	tmpstr = strchr(curstr, ':');

	/* Get the scheme length */
	len = NULL == tmpstr ? 0 : tmpstr - curstr;

	/* Is scheme there */
	if (len != 0) {
		/* Check restrictions */
		int is_scheme = 1;
		for (i = 0; i < len; i++) {
			if (!_is_scheme_char(curstr[i])) {
				/* Invalid format */
				is_scheme = 0;
			}
		}

		if (is_scheme) {
			/* Copy the scheme to the storage */
			purl->scheme = malloc(sizeof(char) * (len + 1));
			if (NULL == purl->scheme) {
				parsed_url_free(purl);
				return NULL;
			}
			(void) strncpy(purl->scheme, curstr, len);
			purl->scheme[len] = '\0';
			/* Make the character to lower if it is upper case. */
			for (i = 0; i < len; i++) {
				purl->scheme[i] = tolower(purl->scheme[i]);
			}
			/* Skip ':' */
			tmpstr++;
			curstr = tmpstr;
		}
	}

	/*
	 * //<user>:<password>@<host>:<port>/<url-path>
	 * Any ":", "@" and "/" must be encoded.
	 */
	/* Eat "//" */
	if ('/' == curstr[0] && '/' == curstr[1]) {
		curstr++;
		curstr++;

		/* Check if the user (and password) are specified. */
		userpass_flag = 0;
		tmpstr = curstr;
		while ('\0' != *tmpstr) {
			if ('@' == *tmpstr) {
				/* Username and password are specified */
				userpass_flag = 1;
				break;
			} else if ('/' == *tmpstr) {
				/* End of <host>:<port> specification */
				userpass_flag = 0;
				break;
			}
			tmpstr++;
		}

		/* User and password specification */
		tmpstr = curstr;
		if (userpass_flag) {
			/* Read username */
			while ('\0' != *tmpstr && ':' != *tmpstr && '@' != *tmpstr) {
				tmpstr++;
			}
			len = tmpstr - curstr;
			purl->username = malloc(sizeof(char) * (len + 1));
			if (NULL == purl->username) {
				parsed_url_free(purl);
				return NULL;
			}
			(void) strncpy(purl->username, curstr, len);
			purl->username[len] = '\0';
			/* Proceed current pointer */
			curstr = tmpstr;
			if (':' == *curstr) {
				/* Skip ':' */
				curstr++;
				/* Read password */
				tmpstr = curstr;
				while ('\0' != *tmpstr && '@' != *tmpstr) {
					tmpstr++;
				}
				len = tmpstr - curstr;
				purl->password = malloc(sizeof(char) * (len + 1));
				if (NULL == purl->password) {
					parsed_url_free(purl);
					return NULL;
				}
				(void) strncpy(purl->password, curstr, len);
				purl->password[len] = '\0';
				curstr = tmpstr;
			}
			/* Skip '@' */
			if ('@' != *curstr) {
				parsed_url_free(purl);
				return NULL;
			}
			curstr++;
		}
	}

	if ('/' != *curstr) {
		if ('[' == *curstr) {
			bracket_flag = 1;
		} else {
			bracket_flag = 0;
		}
		/* Proceed on by delimiters with reading host */
		tmpstr = curstr;
		while ('\0' != *tmpstr) {
			if (bracket_flag && ']' == *tmpstr) {
				/* End of IPv6 address. */
				tmpstr++;
				break;
			} else if (!bracket_flag && (':' == *tmpstr || '/' == *tmpstr)) {
				/* Port number is specified. */
				break;
			}
			tmpstr++;
		}
		len = tmpstr - curstr;
		purl->host = malloc(sizeof(char) * (len + 1));
		if (NULL == purl->host || len <= 0) {
			parsed_url_free(purl);
			return NULL;
		}
		(void) strncpy(purl->host, curstr, len);
		purl->host[len] = '\0';
		curstr = tmpstr;

		/* Is port number specified? */
		if (':' == *curstr) {
			curstr++;
			/* Read port number */
			tmpstr = curstr;
			while ('\0' != *tmpstr && '/' != *tmpstr) {
				tmpstr++;
			}
			len = tmpstr - curstr;
			purl->port = malloc(sizeof(char) * (len + 1));
			if (NULL == purl->port) {
				parsed_url_free(purl);
				return NULL;
			}
			(void) strncpy(purl->port, curstr, len);
			purl->port[len] = '\0';
			curstr = tmpstr;
		}

		/* End of the string */
		if ('\0' == *curstr) {
			return purl;
		}
	}

	/* Skip '/' */
	if ('/' != *curstr) {
		parsed_url_free(purl);
		return NULL;
	}
	curstr++;

	/* Parse path */
	tmpstr = curstr;
	while ('\0' != *tmpstr && '#' != *tmpstr && '?' != *tmpstr) {
		tmpstr++;
	}
	len = tmpstr - curstr;
	purl->path = malloc(sizeof(char) * (len + 1));
	if (NULL == purl->path) {
		parsed_url_free(purl);
		return NULL;
	}
	(void) strncpy(purl->path, curstr, len);
	purl->path[len] = '\0';
	curstr = tmpstr;

	/* Is query specified? */
	if ('?' == *curstr) {
		/* Skip '?' */
		curstr++;
		/* Read query */
		tmpstr = curstr;
		while ('\0' != *tmpstr && '#' != *tmpstr) {
			tmpstr++;
		}
		len = tmpstr - curstr;
		purl->query = malloc(sizeof(char) * (len + 1));
		if (NULL == purl->query) {
			parsed_url_free(purl);
			return NULL;
		}
		(void) strncpy(purl->query, curstr, len);
		purl->query[len] = '\0';
		curstr = tmpstr;
	}

	/* Is fragment specified? */
	if ('#' == *curstr) {
		/* Skip '#' */
		curstr++;
		/* Read fragment */
		tmpstr = curstr;
		while ('\0' != *tmpstr) {
			tmpstr++;
		}
		len = tmpstr - curstr;
		purl->fragment = malloc(sizeof(char) * (len + 1));
		if (NULL == purl->fragment) {
			parsed_url_free(purl);
			return NULL;
		}
		(void) strncpy(purl->fragment, curstr, len);
		purl->fragment[len] = '\0';
		curstr = tmpstr;
	}

	return purl;
}

/*
 * Free memory of parsed url
 */
void parsed_url_free(struct parsed_url *purl) {
	if (NULL != purl) {
		if (NULL != purl->scheme) {
			free(purl->scheme);
		}
		if (NULL != purl->host) {
			free(purl->host);
		}
		if (NULL != purl->port) {
			free(purl->port);
		}
		if (NULL != purl->path) {
			free(purl->path);
		}
		if (NULL != purl->query) {
			free(purl->query);
		}
		if (NULL != purl->fragment) {
			free(purl->fragment);
		}
		if (NULL != purl->username) {
			free(purl->username);
		}
		if (NULL != purl->password) {
			free(purl->password);
		}
		free(purl);
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
