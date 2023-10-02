/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.02.2013
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include <security/smac/smac.h>


extern int cmd_smac_adm_user_set(const char *name, const char *label);
extern int cmd_smac_adm_user_get(const char *name, char *buf, size_t buflen);

#define BUFLEN 4096

static char buf[BUFLEN];

static int print_rules(void) {
	struct smac_env *env;
	struct smac_entry *ent;
	int res, i;

	if (0 != (res = smac_getenv(buf, BUFLEN, &env))) {
		return res;
	}

	printf("Total rules: %d\n", env->n);

	for (ent = env->entries, i = env->n; i > 0; ++ent, --i) {
		printf("%16s %16s    ", ent->subject, ent->object);
		putchar(ent->flags & S_IROTH  ? 'r' : '-');
		putchar(ent->flags & S_IWOTH ? 'w' : '-');
		putchar(ent->flags & S_IXOTH ?  'x' : '-');
		putchar('\n');
	}

	return 0;
}

static int new_rule(const char *subject, const char *object,
		const char *access) {
	int flags = 0;

	flags |= strchr(access, 'r') ? S_IROTH  : 0;
	flags |= strchr(access, 'w') ? S_IWOTH : 0;
	flags |= strchr(access, 'x') ? S_IXOTH  : 0;

	return smac_addenv(subject, object, flags);
}

static int print_label(const char *name) {
	int res;

	if (name == NULL) {
		res = smac_labelget(buf, BUFLEN);
	} else {
		res = cmd_smac_adm_user_get(name, buf, BUFLEN);
	}

	if (0 != res) {
		return res;
	}

	puts(buf);

	return 0;
}

int main(int argc, char *argv[]) {
	char *label = NULL;
	char *user = NULL;
	char *subject = NULL, *object = NULL, *access = NULL;
	enum action {
		ACT_NONE,
		ACT_SET,
		ACT_GET,
		ACT_FLUSH,
		ACT_PRINT,
		ACT_RULE,
		ACT_USER,
		ACT_HELP,
	} action = ACT_NONE;
	int opt;

	while (-1 != (opt = getopt(argc, argv, "S:GFPR:U:o:a:h"))) {
		enum action act = ACT_NONE;

		switch(opt) {
		case 'S':
			act = ACT_SET;
			label = optarg;
			break;
		case 'G':
			act = ACT_GET;
			break;
		case 'F':
			act = ACT_FLUSH;
			break;
		case 'P':
			act = ACT_PRINT;
			break;
		case 'R':
			act = ACT_RULE;
			subject = optarg;
			break;
		case 'U':
			act = ACT_USER;
			user = optarg;
			label = argv[optind++];
			break;
		case 'o':
			object = optarg;
			break;
		case 'a':
			access = optarg;
			break;
		case 'h':
			act = ACT_HELP;
			break;
		default:
			printf("Unknown argument -- %c\n", optopt);
			return -EINVAL;
		}

		if (act != ACT_NONE) {
			if (action != ACT_NONE) {
				printf("Incorrect commmand line, multiplie action specified\n");
				return -EINVAL;
			}

			action = act;
		}
	}

	switch(action) {
	case ACT_SET:
		return smac_labelset(label);
	case ACT_GET:
		return print_label(optind < argc ? argv[optind] : NULL);
	case ACT_FLUSH:
		return smac_flushenv();
	case ACT_RULE:
		return new_rule(subject, object, access);
	case ACT_PRINT:
		return print_rules();
	case ACT_USER:
		return cmd_smac_adm_user_set(user, label);
	case ACT_NONE:
	default:
		break;
	}
	return 0;
}
