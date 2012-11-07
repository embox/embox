/*
 * Copyright 2010 Scyphus Solutions Co. Ltd.  All rights reserved.
 *
 * Authors:
 *      Hirochika Asai
 *
 *  Modified on: Oct 31, 2012
 *      Author: vita
 */

#ifndef URL_PARSER_H_
#define URL_PARSER_H_

/*
 * URL storage
 */
struct parsed_url {
	/*all optional*/
	char *scheme;
	char *host;
	char *port;
	char *path;
	char *query;
	char *fragment;
	char *username;
	char *password;
};

/**
 * parse absolute uri or absolute path
 *
 * @param uri string
 * @retval parsed_url on success
 * @retval NULL on fail parsing
 */
struct parsed_url * parse_url(const char *);
/**
 * Free struct parsed_url
 */
void parsed_url_free(struct parsed_url *);

#endif /* URL_PARSER_H_ */
