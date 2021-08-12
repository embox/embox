#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

extern struct demo_call_info *call_info;

int main(int argc, char *argv[]) {
	call_info = malloc(sizeof (struct demo_call_info));

	memset(call_info, 0, sizeof *call_info);

	demo_pj_main(1, NULL, demo_nk_main, argv[argc - 1]);

	//demo_nk_main(argv[argc - 1]);

	free(call_info);

	return 0;
}
