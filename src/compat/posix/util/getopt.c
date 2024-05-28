/**
 * @file
 * @brief
 *
 * @date 23.09.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <string.h>
#include <unistd.h>

extern int getopt_sp;
extern int getopt_not_opt;

/**
 * @param argc is the number of arguments on cmdline
 * @param argv is the pointer to array of cmdline arguments
 * @param opts is the string of all valid options
 * each char case must be given; options taking an arg are followed by = ':'
 */
int getopt(int argc, char *const argv[], const char *opts) {
	int c;
	char *cp;

	if (getopt_sp == 1) {
		/* check for end of options */
		while (optind < argc
		       && (argv[optind][0] != '-' || argv[optind][1] == '\0')) {
			optind++;
			getopt_not_opt++;
		}
		if (optind >= argc) {
			optarg = NULL;
			optind -= getopt_not_opt;
			getopt_sp = 1;
			getopt_not_opt = 0;
			return -1;
		}
		else if (!strcmp(argv[optind], "--")) {
			optind++;
			optarg = NULL;
			optind -= getopt_not_opt;
			getopt_sp = 1;
			getopt_not_opt = 0;
			return -1;
		}
	}
	c = argv[optind][getopt_sp];
	if (c == ':' || (cp = strchr(opts, c)) == NULL) {
		/* if arg sentinel as option or other invalid option,
		handle the error and return '?' */
		if (argv[optind][++getopt_sp] == '\0') {
			optind++;
			getopt_sp = 1;
		}
		optopt = c;
		return '?';
	}
	if (*++cp == ':') {
		/* if option is given an argument...  */
		if (argv[optind][getopt_sp + 1] != '\0')
			/* and the OptArg is in that CmdLineArg, return it... */
			optarg = &argv[optind++][getopt_sp + 1];
		else if (++optind >= argc) {
			/* but if the OptArg isn't there and the next CmdLineArg
			 isn't either, handle the error... */
			getopt_sp = 1;
			optopt = c;
			return '?';
		}
		else
			/* but if there is another CmdLineArg there, return that */
			optarg = argv[optind++];
		/* and set up for the next CmdLineArg */
		getopt_sp = 1;
	}
	else {
		/* no arg for this opt, so null arg and set up for next option */
		if (argv[optind][++getopt_sp] == '\0') {
			getopt_sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return c;
}
