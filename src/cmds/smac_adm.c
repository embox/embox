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
#include <security/smac.h>
#include <fs/flags.h>
#include <errno.h>

#include <embox/cmd.h>

EMBOX_CMD(smac_adm);

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
		putchar(ent->flags & FS_MAY_READ  ? 'r' : '-');
		putchar(ent->flags & FS_MAY_WRITE ? 'w' : '-');
		putchar(ent->flags & FS_MAY_EXEC ?  'x' : '-');
		putchar('\n');
	}

	return 0;
}

static int new_rule(const char *subject, const char *object,
		const char *access) {
	int flags = 0;

	flags |= strchr(access, 'r') ? FS_MAY_READ  : 0;
	flags |= strchr(access, 'w') ? FS_MAY_WRITE : 0;
	flags |= strchr(access, 'x') ? FS_MAY_EXEC  : 0;

	return smac_addenv(subject, object, flags);
}

static int print_label(void) {
	int res;

	if (0 != (res = smac_labelget(buf, BUFLEN))) {
		return res;
	}

	puts(buf);

	return 0;
}

static int smac_adm(int argc, char *argv[]) {
	char *lset, *rule, *object, *access, *bp;
	int opt;
	enum action {
		ACT_NONE,
		ACT_SET,
		ACT_GET,
		ACT_FLUSH,
		ACT_PRINT,
		ACT_RULE,
		ACT_HELP,
	} action = ACT_NONE;

	lset = rule = object = access = NULL;
	bp = buf;
	action = 0;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "S:GFPR:o:a:h"))) {
		enum action act = ACT_NONE;
		char **arg = NULL;

		switch(opt) {
		case 'S':
			act = ACT_SET;
			arg = &lset;
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
			arg = &rule;
			break;
		case 'o':
			arg = &object;
			break;
		case 'a':
			arg = &access;
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

		if (arg) {
			*arg = bp;
			strcpy(bp, optarg);
			bp += 1 + strlen(optarg);
		}
	}

	switch(action) {
	case ACT_SET:
		return smac_labelset(lset);
	case ACT_GET:
		return print_label();
	case ACT_FLUSH:
		return smac_flushenv();
	case ACT_RULE:
		return new_rule(rule, object, access);
	case ACT_PRINT:
		return print_rules();
	case ACT_NONE:
	default:
		break;
	}
	return 0;
}
