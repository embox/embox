/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.05.2014
 */

#include <stdlib.h>
#include <kernel/task.h>

#include <embox/cmd.h>

EMBOX_CMD(dropbeard_main);

static void *dropbeard(void *arg) {

	while (1) {
		system("dropbear_new -F");
	}

	return NULL;
}

static int dropbeard_main(int argc, char *argv[]) {

	new_task("dropbeard", dropbeard, NULL);

	return 0;
}

