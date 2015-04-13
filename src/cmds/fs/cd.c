/**
 * @file
 * @brief
 *
 * @date 01.04.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <framework/mod/options.h>
#include <unistd.h>

#define MODOPS_HOME_DIR OPTION_STRING_GET(home_dir)

int main(int argc, char *argv[]) {
	char *path = argc < 2 ? MODOPS_HOME_DIR : argv[1];

	if (-1 == chdir(path))
		return -errno;

	return 0;
}
