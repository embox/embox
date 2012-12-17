#include <lib/service/jsp.h>
#include <stdio.h>
#include <stdlib.h>

#define JSP_SENTINEL '@'
#define JSP_TEXT_PREFIX "\nfprintf(SERVICE_OUTPUT, \""
#define JSP_TEXT_POSTFIX "\");\n"

extern int  service_jsp_interpret_init (int heap_size) {
	return 0;
}
extern void service_jsp_interpret_fini (void) {
}
extern void service_jsp_preprocess (FILE * in, FILE * out) {
	char t;
	int jsp_code = 0; /*Bare text or jsp-code*/
	int section_opened = 0;
	int escaped = 0;

	if( !in ) {
		return;
	}
	fprintf(out, "#include <stdio.h>\nint main () {");

	if (!jsp_code && !section_opened) {
		fprintf(out, JSP_TEXT_PREFIX);
		section_opened = 1;
	}
	for (t = fgetc(in); t != EOF; t = fgetc(in)) {
		if (t == JSP_SENTINEL) {
			if (escaped) {
				fputc(JSP_SENTINEL, out);
				escaped = 0;
			} else {
				escaped = 1;
			}
		} else {
			if (escaped) {
				if (jsp_code) {
					fprintf(out, JSP_TEXT_PREFIX);
					jsp_code = 0;
				} else {
					fprintf(out, JSP_TEXT_POSTFIX);
					jsp_code = 1;
				}
				fputc(t, out);
				escaped = 0;
			} else {
				fputc(t, out);
			}
		}
	}
	if (!jsp_code && section_opened) {
		fprintf(out, JSP_TEXT_POSTFIX);
		section_opened = 0;
	}
	fprintf(out, "\nreturn 0;\n}");
}

extern void service_jsp_interpret (struct service_file * file) {
}
