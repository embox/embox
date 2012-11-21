/**
 * @file
 *
 * @date Nov 8, 2012
 * @author: Vita Loginova
 */

#include <embox/web_service.h>
#include <stdio.h>
#include <string.h>
#include <net/util/request_parser.h>
#include <lib/expat.h>

#define BUFF_SZ       (1460 * 2)
#define FILENAME_SZ   30

enum http_method {
	HTTP_METHOD_UNKNOWN = 0, HTTP_METHOD_GET, HTTP_METHOD_POST
};

enum http_content_type {
	HTTP_CONTENT_TYPE_HTML = 0,
	HTTP_CONTENT_TYPE_JPEG,
	HTTP_CONTENT_TYPE_PNG,
	HTTP_CONTENT_TYPE_GIF,
	HTTP_CONTENT_TYPE_ICO,
	HTTP_CONTENT_TYPE_UNKNOWN,
	HTTP_CONTENT_TYPE_MAX
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
	http_request * parsed; /* parsed request */
	struct event unlock_sock_event;
	int lock_status;
};

struct params {
	struct client_info* info;
	char *text;
};

static void XMLCALL
startElement(void *userData, const char *name, const char **atts) {
	struct params *data = (struct params *) userData;
	FILE *file = data->info->fp;
	int i = 0;

	if (strcmp(name, "c:out") == 0) {
		//param -> text
		fwrite(data->text, sizeof(char), strlen(data->text), file);
	} else {
		//open tag
		fwrite("<", sizeof(char), 1, file);
		fwrite(name, sizeof(char), strlen(name), file);

		//add atts
		while (atts[i] != NULL) {
			fwrite(" ", sizeof(char), 1, file);
			fwrite(atts[i], sizeof(char), strlen(atts[i]), file);
			fwrite("=\"", sizeof(char), 2, file);
			i++;
			fwrite(atts[i], sizeof(char), strlen(atts[i]), file);
			fwrite("\"", sizeof(char), 1, file);
			i++;
		}

		fwrite(">", sizeof(char), 1, file);
	}
}

static void XMLCALL
endElement(void *userData, const char *name) {
	struct params *data = (struct params *) userData;
	FILE *file = data->info->fp;

	if (strcmp(name, "c:out") != 0) {
		//close tag
		fwrite("</", sizeof(char), 2, file);
		fwrite(name, sizeof(char), strlen(name), file);
		fwrite(">", sizeof(char), 1, file);
	}
}

static void XMLCALL
character(void *userData, const XML_Char *s, int len) {
	struct params *data = (struct params *) userData;
	FILE *file = data->info->fp;

	//text between tags
	fwrite(s, sizeof(char), len, file);
}

static void *process_params(void* args) {
	struct web_service_instance *inst;
	struct params *params;
	char buf[512];
	FILE *file;
	int done;
	XML_Parser parser;

	inst = (struct web_service_instance *) args;
	params = (struct params *) inst->params;

	file = fopen(params->info->file, "r");
	if (file == NULL) { /* file doesn't exist */
		return NULL;
	}
	params->info->fp = fopen("/tmp/test_temp.html", "w");
	if (params->info->fp == NULL) { /* file doesn't exist */
		fclose(file);
		return NULL;
	}

	parser = XML_ParserCreate(NULL);
	XML_SetHTMLUse(parser);
	XML_SetUserData(parser, params);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, character);

	do {
		int len = (int) fread(buf, 1, sizeof(buf), file);
		done = len < sizeof(buf);
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
			fclose(file);
			fclose(params->info->fp);
			params->info->fp = NULL;
			params->info->lock_status = 0;
			event_notify(&params->info->unlock_sock_event);
			return NULL;
		}
	} while (!done);

	XML_ParserFree(parser);
	fclose(file);
	fclose(params->info->fp);

	params->info->lock_status = 0;
	event_notify(&params->info->unlock_sock_event);
	params->info->fp = fopen("/tmp/test_temp.html", "r");

	return NULL;

}

EMBOX_WEB_SERVICE("test.html", process_params);
