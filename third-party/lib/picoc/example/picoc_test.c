#include <lib/picoc.h>
#include <framework/example/self.h>

EMBOX_EXAMPLE(start);

static int start(int argc, char *argv[]) {
	char *source[] = { "picoc", "hello_world" };
	return picoc_main(2, source);
	return 0;
}

