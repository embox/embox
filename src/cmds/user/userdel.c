/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    27.02.2014
 */

#include <unistd.h>
#include <ctype.h>

#include "user.h"

static int shadow_del(char *name) {
	struct spwd *spwd;
	FILE *shdwf, *temp_shdwf;

	if (0 == (shdwf = fopen(SHADOW_FILE, "r"))) {
		return -1;
	}

	if (0 == (temp_shdwf = fopen(TMP_SHADOW_FILE, "w"))) {
		return -1;
	}

	while (NULL != (spwd = fgetspent(shdwf))) {
		if (0 == strcmp(spwd->sp_namp, name)) {
			continue;
		}
		user_write_user_spwd(spwd, temp_shdwf);
	}

	fclose(temp_shdwf);
	fclose(shdwf);

	user_copy(TMP_SHADOW_FILE, SHADOW_FILE);
	remove(TMP_SHADOW_FILE);

	return 0;
}

static int passwd_del(char *name) {
	struct passwd pwd, *pwd_res;
	FILE *pswdf, *temp_pswdf;
	char buf_pswd[80];
	int res = 0;

	if (0 == (pswdf = fopen(PASSWD_FILE, "r"))) {
		return -1;
	}

	if (0 == (temp_pswdf = fopen(TMP_PASSWD_FILE, "w"))) {
		return -1;
	}

	while (0 == (res = fgetpwent_r(pswdf, &pwd, buf_pswd, 80, &pwd_res))) {
		if (0 == strcmp(pwd.pw_name, name)) {
			continue;
		}
		user_write_user_passwd(&pwd, temp_pswdf);
	}

	fclose(temp_pswdf);
	fclose(pswdf);

	user_copy(TMP_PASSWD_FILE, PASSWD_FILE);
	remove(TMP_PASSWD_FILE);

	return 0;
}

static void print_help(void) {
	printf("Usage:\tuserdel [option] LOGIN\n"
			"Options: see 'man usage'\n");
}

int main(int argc, char **argv) {
	char name[15];
	int opt;

	if (argc >= 1) {
		getopt_init();
		while (-1 != (opt = getopt(argc, argv, "h"))) {

			switch (opt) {
			case 'h':
				goto out;
			default:
				printf("userdel: invalid option -%c\n", optopt);
				goto out;
			}
		}

		if (optind >= argc) {
			goto out;
		}

		strcpy(name, argv[optind]);

		if (!user_is_user_exists(name)) {
			printf("userdel: user '%s' doesn't exist\n", name);
			return 0;
		}

		passwd_del(name);
		shadow_del(name);
		return 0;
	}

out:
	print_help();
	return 0;
}
