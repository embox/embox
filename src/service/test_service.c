/**
 * @file
 *
 * @date Nov 8, 2012
 * @author: Vita Loginova
 */

#include <embox/web_service.h>
#include <string.h>
#include <lib/expat.h>

static void start_element(void *userData, const char *name, const char **atts) {
	struct params *data = (struct params *) userData;
	FILE *file = data->info->fp;
	int i = 0;

	if (strcmp(name, "c:out") == 0) {
		//param -> text
		fwrite(data->query, sizeof(char), strlen(data->query), file);
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

static void end_element(void *userData, const char *name) {
	struct params *data = (struct params *) userData;
	FILE *file = data->info->fp;

	if (strcmp(name, "c:out") != 0) {
		//close tag
		fwrite("</", sizeof(char), 2, file);
		fwrite(name, sizeof(char), strlen(name), file);
		fwrite(">", sizeof(char), 1, file);
	}
}

static void character(void *userData, const XML_Char *s, int len) {
	struct params *data = (struct params *) userData;
	FILE *file = data->info->fp;

	//text between tags
	fwrite(s, sizeof(char), len, file);
}

static void *process_params(void* args) {
	struct web_service_instance *inst;
	struct params *params;
	char temp_file[] = "/tmp/test_temp.html";
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
	XML_SetElementHandler(parser, start_element, end_element);
	XML_SetCharacterDataHandler(parser, character);

	do {
		int len = (int) fread(buf, 1, sizeof(buf), file);
		done = len < sizeof(buf);
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
			fclose(file);
			fclose(params->info->fp);
			params->info->fp = NULL;
			params->info->lock_status = 0;

			return NULL;
		}
	} while (!done);

	XML_ParserFree(parser);
	fclose(file);
	fclose(params->info->fp);

	//params->info->fp = fopen("/tmp/test_temp.html", "r");
	params->info->fp =  NULL;
	strcpy(params->info->file, temp_file);
	params->info->file[strlen(temp_file)] = '\0';

	return NULL;
}

EMBOX_WEB_SERVICE("test.html", process_params);
