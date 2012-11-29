#include <embox/web_service.h>
#include <string.h>

static void * entry_point (void * args) {
	struct web_service_instance * inst;
	struct params * params;
	char buf[512] = "I'm the great cornholio, I need a TP for my bumhole!";
	const char tfile_name[] = "/tmp/img_serv_temp.html";
	FILE * file;

	inst = (struct web_service_instance *) args;
	params = (struct params * ) inst->params;

	file = fopen (tfile_name, "w");
	fwrite (buf, sizeof(buf), 1, file);
	fclose (file);
	params->info->fp = NULL;
	strcpy(params->info->file, tfile_name);

	return NULL;
}

EMBOX_WEB_SERVICE("img.html", entry_point);
