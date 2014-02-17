/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    13.02.2014
 */


#include <embox/cmd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>

#define BUF_LEN 64

EMBOX_CMD(useradd);

#define PASSWD_FILE "/tmp/passwd"
#define SHADOW_FILE "/tmp/shadow"

static void get_default_pwd(struct passwd *result, char *name) {
	result->pw_name = name;
	result->pw_passwd = "x";
	result->pw_uid = getmaxuid() + 1;
	result->pw_gid = 1001;
	result->pw_gecos = name;
	result->pw_dir = "/";
	result->pw_shell = "/bin/false";
}

static int create(char *name) {
	struct passwd pwd;
	FILE *pswdf, *sdwf;
	char buf[40];

	if (0 == (pswdf = fopen(PASSWD_FILE, "a"))) {
		return -1;
	}

	if (0 == (sdwf = fopen(SHADOW_FILE, "a"))) {
		fclose(sdwf);
		return -1;
	}

	get_default_pwd(&pwd, name);

	fwrite(pwd.pw_name, sizeof(char), strlen(pwd.pw_name), pswdf);
	fputc(':', pswdf);

	fwrite(pwd.pw_passwd, sizeof(char), strlen(pwd.pw_passwd), pswdf);
	fputc(':', pswdf);

	sprintf(buf, "%i", pwd.pw_uid);
	fwrite(buf, sizeof(char), strlen(buf), pswdf);
	fputc(':', pswdf);

	sprintf(buf, "%i", pwd.pw_gid);
	fwrite(buf, sizeof(char), strlen(buf), pswdf);
	fputc(':', pswdf);

	fwrite(pwd.pw_gecos, sizeof(char), strlen(pwd.pw_gecos), pswdf);
	fputc(':', pswdf);

	fwrite(pwd.pw_dir, sizeof(char), strlen(pwd.pw_dir), pswdf);
	fputc(':', pswdf);

	fwrite(pwd.pw_shell, sizeof(char), strlen(pwd.pw_shell), pswdf);
	fputc('\n', pswdf);


	fwrite(pwd.pw_name, sizeof(char), strlen(pwd.pw_name), sdwf);
	fputc(':', sdwf);
	//password
	fwrite(":::::::", sizeof(char), 7, sdwf);
	fputc('\n', sdwf);

	fclose(pswdf);
	fclose(sdwf);

	return 0;
}

static int create_user(char* name) {
	char pwdbuf[BUF_LEN];
	struct passwd pwd, *result;

	if ((getpwnam_r(name, &pwd, pwdbuf, BUF_LEN, &result)) ||
			result) {
		//user exists
		return -1;
	}

	return create(name);
}

static int useradd(int argc, char **argv) {
	char *name;
	int opt;

	if (argc != 1) {

		getopt_init();

		while (-1 != (opt = getopt(argc, argv, "pc:"))) {
			switch(opt) {
			default:
				break;
			}
		}

		if (optind >= argc) {
			//help
			return 0;
		}

		name = argv[optind];

		return create_user(name);
	}

	//help
	return 0;
}
