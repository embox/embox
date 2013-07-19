#include <framework/example/self.h>
#include <lib/service/jsp.h>
#include <stdlib.h>

#define JSP_TEST_IN_FILE "jsp_test.in"
#define JSP_TEST_OUT_FILE "/tmp/jsp.out"

EMBOX_EXAMPLE(start);

static int start(int argc, char ** argv) {
	FILE *in, *out;
	out = fopen(JSP_TEST_OUT_FILE, "w");
	in = fopen(JSP_TEST_IN_FILE, "r");
	service_jsp_preprocess(in, out);
	fclose (out);
	fclose (in);
	return 0;
}
