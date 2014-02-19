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
#include <grp.h>
#include <ctype.h>

#define BUF_LEN 64

EMBOX_CMD(useradd);

#define PASSWD_FILE "/tmp/passwd"
#define SHADOW_FILE "/tmp/shadow"
#define ADDUSER_FILE "/tmp/adduser.conf"

static int get_default_pwd(struct passwd *result, char *name, char *buf,
		size_t buf_len) {

	/* set GROUP, HOME and SHELL */
	if (0 != get_defpswd(result, buf, buf_len)) {
		return -1;
	}

	result->pw_name = name;
	result->pw_passwd = "x";
	result->pw_uid = getmaxuid() + 1;
	result->pw_gecos = name;

	return 0;
}

static int set_options(struct passwd *result, char *home, char *shell,
		char *gecos, char *group) {
	if (0 != strcmp(home, "")) {
		result->pw_dir = home;
	}

	if (0 != strcmp(shell, "")) {
		result->pw_shell = shell;
	}

	if (0 != strcmp(gecos, "")) {
		result->pw_gecos = gecos;
	}

	if (0 != strcmp(group, "")) {
		struct group _group, *_group_res;
		char buf[80];
		int res = 0;

		if (isdigit(group[0])) {
			res = getgrgid_r(atoi(group), &_group, buf, 80, &_group_res);
		} else {
			res = getgrnam_r(group, &_group, buf, 80, &_group_res);
		}
		if (!res && _group_res != NULL) {
			result->pw_gid = _group_res->gr_gid;
		} else {
			printf("useradd: group '%s' doesn't exist\n", group);
			return -1;
		}
	}

	return 0;
}

static int create_user(char *name, char *home, char *shell, char *pswd,
		char *gecos, char *group) {
	struct passwd pwd;
	FILE *pswdf, *sdwf;
	char buf_int[40], buf_pswd[80];
	int res = 0;

	if (0 == (pswdf = fopen(PASSWD_FILE, "a"))) {
		return -1;
	}

	if (0 == (sdwf = fopen(SHADOW_FILE, "a"))) {
		fclose(sdwf);
		return -1;
	}

	if (0 != get_default_pwd(&pwd, name, buf_pswd, 80)) {
		res = -1;
		goto out;
	}

	if (0 != set_options(&pwd, home, shell, gecos, group)) {
		goto out;
	}

	/* passwd */
	{
		fwrite(pwd.pw_name, sizeof(char), strlen(pwd.pw_name), pswdf);
		fputc(':', pswdf);

		fwrite(pwd.pw_passwd, sizeof(char), strlen(pwd.pw_passwd), pswdf);
		fputc(':', pswdf);

		sprintf(buf_int, "%i", pwd.pw_uid);
		fwrite(buf_int, sizeof(char), strlen(buf_int), pswdf);
		fputc(':', pswdf);

		sprintf(buf_int, "%i", pwd.pw_gid);
		fwrite(buf_int, sizeof(char), strlen(buf_int), pswdf);
		fputc(':', pswdf);

		fwrite(pwd.pw_gecos, sizeof(char), strlen(pwd.pw_gecos), pswdf);
		fputc(':', pswdf);

		fwrite(pwd.pw_dir, sizeof(char), strlen(pwd.pw_dir), pswdf);
		fputc(':', pswdf);

		fwrite(pwd.pw_shell, sizeof(char), strlen(pwd.pw_shell), pswdf);
		fputc('\n', pswdf);
	}

	/* shadow */
	{
		fwrite(pwd.pw_name, sizeof(char), strlen(pwd.pw_name), sdwf);
		fputc(':', sdwf);
		fwrite(pswd, sizeof(char), strlen(pswd), sdwf);
		fwrite(":::::::", sizeof(char), 7, sdwf);
		fputc('\n', sdwf);
	}

out: fclose(pswdf);
	fclose(sdwf);
	return res;
}

static int is_user_exists(char *name) {
	char pwdbuf[BUF_LEN];
	struct passwd pwd, *result;

	if ((getpwnam_r(name, &pwd, pwdbuf, BUF_LEN, &result)) || result) {
		return 1;
	}

	return 0;
}

static void print_help(void) {
	printf("Usage:\tuseradd [option] LOGIN\n"
			"\tuseradd -D\n"
			"Options: see 'man usage'\n");
}

static int print_default_options(void) {
	FILE *fd;
	char buff;
	int number = 0, line = 0, new_line = 1;

	if (NULL == (fd = fopen(ADDUSER_FILE, "r"))) {
		return -1;
	}

	while (fread(&buff, 1, 1, fd) == 1) {
		if (new_line && number) {
			printf("\t%d %c", line++, buff);
		} else {
			printf("%c", buff);
		}
		new_line = (buff == '\n') ? 1 : 0;
	}
	fclose(fd);

	return 0;
}

static int useradd(int argc, char **argv) {
	char name[15], home[20] = "", shell[20] = "", pswd[15] = "", gecos[15] = "",
			group[15] = "";
	int opt, count = 0;

	if (argc >= 1) {
		getopt_init();
		while (-1 != (opt = getopt(argc, argv, "d:s:p:c:g:Dh"))) {
			count++;

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
			case 'g':
				strcpy(group, optarg);
				break;
			case 'h':
				print_help();
				return 0;
			case 'D':
				if (count != 1) {
					print_help();
					return 0;
				}
				if (optind >= argc) {
					return print_default_options();
				}
				break;
			default:
				printf("useradd: invalid option -%c\n", optopt);
				print_help();
				return 0;
			}
		}

		if (optind >= argc) {
			print_help();
			return 0;
		}

		strcpy(name, argv[optind]);

		if (is_user_exists(name)) {
			printf("useradd: user '%s' already exists\n", name);
			return 0;
		}
		return create_user(name, home, shell, pswd, gecos, group);
	}

	print_help();
	return 0;
}
