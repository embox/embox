/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    24.02.2014
 */

#include <embox/cmd.h>

#include <unistd.h>
#include <ctype.h>

#include "user.h"

#define TMP_PASSWD_FILE "/tmp/passwd.temp"
#define TMP_SHADOW_FILE "/tmp/shadow.temp"

EMBOX_CMD(exec);

static int copy(const char* from, const char* to) {
	FILE *fromf, *tof;
	int a;

	if (NULL == (fromf = fopen(from, "r"))) {
		return -1;
	}

	if (NULL == (tof = fopen(to, "w"))) {
		fclose(fromf);
		return -1;
	}

	while ((a = fgetc(fromf)) != EOF) {
		fputc(a, tof);
	}

	fclose(fromf);
	fclose(tof);

	return 0;
}

static int shadow(char *name, char *new_name, char *pswd) {
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
			set_options_spwd(spwd, new_name, pswd);
		}
		write_user_spwd(spwd, temp_shdwf);
	}

	fclose(temp_shdwf);
	fclose(shdwf);

	copy(TMP_SHADOW_FILE, SHADOW_FILE);
	remove(TMP_SHADOW_FILE);

	return 0;
}

static int usermod(char *name, char *home, char *shell, char *pswd,
		char *gecos, char *new_name, int group) {
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
			set_options_passwd(&pwd, home, shell, gecos, group);

			if (0 != strcmp(new_name, "")) {
				pwd.pw_name = new_name;
				shadow(name, new_name, pswd);
			} else if (0 != strcmp(pswd, "")) {
				shadow(name, new_name, pswd);
			}
		}
		write_user_passwd(&pwd, temp_pswdf);
	}

	fclose(temp_pswdf);
	fclose(pswdf);

	copy(TMP_PASSWD_FILE, PASSWD_FILE);
	remove(TMP_PASSWD_FILE);

	return 0;
}

static void print_help(void) {
	printf("Usage:\tusermod [option] LOGIN\n"
			"Options: see 'man usage'\n");
}

static int exec(int argc, char **argv) {
	char name[15], home[20] = "", shell[20] = "", pswd[15] = "",
			gecos[15] = "", new_name[15] = "";
	int group = -1;
	int opt;

	if (argc >= 1) {
		getopt_init();
		while (-1 != (opt = getopt(argc, argv, "d:s:p:c:g:l:h"))) {

			switch (opt) {
			case 'd':
				//todo: isdir
				strcpy(home, optarg);
				break;
			case 's':
				//todo: isshell
				strcpy(shell, optarg);
				break;
			case 'p':
				strcpy(pswd, optarg);
				break;
			case 'c':
				strcpy(gecos, optarg);
				break;
			case 'l':
				strcpy(new_name, optarg);
				break;
			case 'g':
				if ((group = get_group(optarg)) < 0) {
					printf("usermod: group '%i' doesn't exist\n", group);
					return 0;
				}
				break;
			case 'h':
				goto out;
			default:
				printf("usermod: invalid option -%c\n", optopt);
				goto out;
			}
		}

		if (optind >= argc) {
			goto out;
		}

		strcpy(name, argv[optind]);

		if (!is_user_exists(name)) {
			printf("usermod: user '%s' doesn't exist\n", name);
			return 0;
		}

		return usermod(name, home, shell, pswd, gecos, new_name, group);
	}

out:
	print_help();
	return 0;
}
