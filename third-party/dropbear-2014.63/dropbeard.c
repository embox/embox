/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.05.2014
 */

#include <stdlib.h>
#include <kernel/task.h>

static void *dropbeard(void *arg) {

	while (1) {
		system("dropbear_new -F");
	}

	return NULL;
}

int main(int argc, char *argv[]) {

	new_task("dropbeard", dropbeard, NULL);

	return 0;
}

