/**
 * @file
 * @brief
 *
 * @date 28.02.13
 * @author Ilia Vaprol
 */

#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	char **env;

	if (!environ) return 0;

	env = environ;
	do
		printf("%s\n", *env);
	while (*++env);

	return 0;
}
