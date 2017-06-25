/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.07.2014
 */

#ifndef GETOPT_H_
#define GETOPT_H_


/**
 * @param argc is the number of arguments on cmdline
 * @param argv is the pointer to array of cmdline arguments
 * @param opts is the string of all valid options
 * each char case must be given; options taking an arg are followed by = ':'
 */
extern int getopt(int argc, char *const argv[], const char *opts);

/** setup optind and opterr */
extern void getopt_init(void); /* TODO remove this */

#define no_argument       0
#define required_argument 1
#define optional_argument 2
struct option {
	const char *name;
	int         has_arg;
	int        *flag;
	int         val;
};

extern int getopt_long(int argc, char * const argv[],
                  const char *optstring,
                  const struct option *longopts, int *longindex);

extern char *optarg; /**< argument to optopt */
extern int optind;   /**< last touched cmdline argument */
extern int optopt;   /**< last returned option */
extern int opterr;   /**< flag:error message on unrecognzed options */

#endif /* GETOPT_H_ */

