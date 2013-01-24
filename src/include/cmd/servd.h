/*
 * servd.h
 *
 *  Created on: Nov 21, 2012
 *      Author: vita
 */

#ifndef SERVD_H_
#define SERVD_H_

#include <net/util/request_parser.h>
#include <stdio.h>

#define BUFF_SZ       (1460 * 2) /* http packet size*/
#define FILENAME_SZ   30

/* HTTP Methods */
enum http_method {
	HTTP_METHOD_UNKNOWN = 0, HTTP_METHOD_GET, HTTP_METHOD_POST
};

/* Type of content */
enum http_content_type {
	HTTP_CONTENT_TYPE_HTML = 0,
	HTTP_CONTENT_TYPE_JPEG,
	HTTP_CONTENT_TYPE_PNG,
	HTTP_CONTENT_TYPE_GIF,
	HTTP_CONTENT_TYPE_ICO,
	HTTP_CONTENT_TYPE_UNKNOWN,
	HTTP_CONTENT_TYPE_MAX
};

enum conn_type {
	HTTP_CONNECTION_CLOSE = 0,
	HTTP_CONNECTION_KEEP_ALIVE
};

/* Returns code */
enum http_ret {
	/* HTTP status code */
	/* 1xx Informational */
	/* 2xx Success */
	HTTP_STAT_200 = 0,
	/* 3xx Redirection */
	/* 4xx Client Error */
	HTTP_STAT_400,
	HTTP_STAT_404,
	HTTP_STAT_405,
	HTTP_STAT_408,
	HTTP_STAT_413,
	HTTP_STAT_414,
	/* 5xx Server Error */
	HTTP_STAT_500,
	HTTP_STAT_MAX, /* for implementation */
	/* Other code */
	HTTP_RET_OK = -100, /* ok */
	HTTP_RET_ABORT, /* close connection */
	HTTP_RET_ENDHDR, /* end header section */
	HTTP_RET_HNDOPS /* set options handler */
};

struct client_info {
	int sock; /* socket for client connection*/
	enum http_method method; /* method in request */
	char file[FILENAME_SZ]; /* file to transmit */
	FILE *fp; /* descriptor of `file` */
	enum http_content_type c_type; /* type of contents which will be send */
	char buff[BUFF_SZ]; /* client's buffer (may contains more than one piece of data) */
	char *data; /* pointer to current chunk */
	char *next_data; /* pointer to next piece of data in buffer */
	size_t next_len; /* length of the next chunk */
	http_request parsed_request; /* parsed request */
	int lock_status;
};

struct params {
	struct client_info* info;
	char *query; /*query string*/
};


extern int get_content_type(const char *file_name);

#endif /* SERVD_H_ */
