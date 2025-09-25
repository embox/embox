/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.04.2015
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <net/util/httpd.h>

static const char *ext2type_html[] = { ".html", ".htm", NULL };
static const char *ext2type_jpeg[] = { ".jpeg", ".jpg", NULL };
static const struct ext2type_table_item {
	const char *type;
	const char *ext;
	const char **exts;
} httpd_ext2type_table[] = {
	{ .exts = ext2type_html, .type = "text/html", },
	{ .exts = ext2type_jpeg, .type = "image/jpeg", },
	{ .ext = ".png",         .type = "image/png", },
	{ .ext = ".gif",         .type = "image/gif", },
	{ .ext = ".ico",         .type = "image/vnd.microsoft.icon", },
	{ .ext = ".js",          .type = "application/javascript", },
	{ .ext = ".css",         .type = "text/css", },
	{ .ext = "",             .type = "application/unknown", },
};
static const char *ext2type_unkwown = "plain/text";

const char *httpd_filename2content_type(const char *filename) {
	int i_table;
	const char *file_ext;

	file_ext = strrchr(filename, '.');
	if (file_ext) {
		for (i_table = 0; i_table < ARRAY_SIZE(httpd_ext2type_table); i_table ++) {
			const struct ext2type_table_item *ti = &httpd_ext2type_table[i_table];

			if (ti->ext) {
				if (0 == strcmp(file_ext, ti->ext)) {
					return ti->type;
				}
			}
			if (ti->exts) {
				const char **ext;
				for (ext = ti->exts; *ext != NULL; ext++) {
					if (0 == strcmp(file_ext, *ext)) {
						return ti->type;
					}
				}
			}
		}
	}

	httpd_error("can't determ content type for file: %s", filename);
	return ext2type_unkwown;
}

int httpd_header(const struct client_info *cinfo, int st, const char *msg) {
	FILE *skf = fdopen(cinfo->ci_sock, "rw");

	if (!skf) {
		httpd_error("can't allocate FILE for socket");
		return -ENOMEM;
	}

	fprintf(skf,
		"HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Connection: close\r\n"
		"\r\n",
		st, msg, "text/plain");

	fclose(skf);
	return 0;
}

