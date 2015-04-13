/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2013
 */

#include <unistd.h>
#include <stdio.h>
#include <pwd.h>

#define BUF_LEN 1024

int main(int argc, char **argv) {
	int res;
	uid_t uid = geteuid();
	struct passwd pwd, *result;
	char buf[BUF_LEN];


	if (0 != (res = getpwuid_r(uid, &pwd, buf, BUF_LEN, &result))) {
		return res;
	}

	printf("%s\n", pwd.pw_name);

	return 0;
}
