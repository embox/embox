/**
 * @file
 *
 * @date 23.09.09
 * @author Nikolay Korotky
 */

#include <getopt.h>
#include <string.h>

int   opterr = 1;
int   optind = 1;
int   optopt;
char  *optarg;

int getopt(int argc, char **argv, const char *opts) {
	static int sp = 1;
	int c;
	char *cp;
	if (sp == 1) {
		/* check for end of options */
		if (optind >= argc ||
				(argv[optind][0] != '/' &&
				argv[optind][0] != '-') ||
				argv[optind][1] == '\0') {
			return -1;
		} else if (!strcmp(argv[optind], "--")) {
			optind++;
			return -1;
		}
	}
	optopt = c = argv[optind][sp];
	if (c == ':' || (cp=strchr(opts, c)) == NULL) {
		/* if arg sentinel as option or other invalid option,
		handle the error and return '?' */
		if (argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
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
}
