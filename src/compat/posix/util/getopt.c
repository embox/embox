/**
 * @file
 * @brief
 *
 * @date 23.09.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <unistd.h>
#include <string.h>

int   opterr = 1;
int   optind = 1;
int   optopt = 0;
char  *optarg = NULL;

static int sp = 1;
static int not_opt = 0;

int getopt(int argc, char *const argv[], const char *opts) {
	int c;
	char *cp;

	if (sp == 1) {
		/* check for end of options */
		while (optind < argc
				&& (argv[optind][0] != '-' || argv[optind][1] == '\0')) {
			optind++;
			not_opt++;
		}
		if (optind >= argc) {
			optarg = NULL;
			optind -= not_opt;
			sp = 1;
			not_opt = 0;
			return -1;
		}
		else if (!strcmp(argv[optind], "--")) {
			optind++;
			optarg = NULL;
			optind -= not_opt;
			sp = 1;
			not_opt = 0;
			return -1;
		}
	}
	c = argv[optind][sp];
	if (c == ':' || (cp=strchr(opts, c)) == NULL) {
		/* if arg sentinel as option or other invalid option,
		handle the error and return '?' */
		if (argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		optopt = c;
		return '?';
	}
	if (*++cp == ':') {
		/* if option is given an argument...  */
		if (argv[optind][sp+1] != '\0')
			/* and the OptArg is in that CmdLineArg, return it... */
			optarg = &argv[optind++][sp+1];
		else if (++optind >= argc) {
			/* but if the OptArg isn't there and the next CmdLineArg
			 isn't either, handle the error... */
			sp = 1;
			optopt = c;
			return '?';
		} else
			/* but if there is another CmdLineArg there, return that */
			optarg = argv[optind++];
		/* and set up for the next CmdLineArg */
		sp = 1;
	} else {
		/* no arg for this opt, so null arg and set up for next option */
		if (argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return c;
}

void getopt_init() {
	opterr = 1;
	optind = 1;
	optopt = 0;
	optarg = NULL;
	sp = 1;
	not_opt = 0;
}
