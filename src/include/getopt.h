/**
 * @file
 *
 * @date 23.09.09
 * @author Nikolay Korotky
 * NOTE: long options not supported.
 */
#ifndef _GETOPT_H
#define _GETOPT_H

extern char *optarg; /**< argument to optopt */
extern int optind;   /**< last touched cmdline argument */
extern int optopt;   /**< last returned option */
extern int opterr;   /**< flag:error message on unrecognzed options */

/**
 * @param argc is the number of arguments on cmdline
 * @param argv is the pointer to array of cmdline arguments
 * @param opts is the string of all valid options
 * each char case must be given; options taking an arg are followed by = ':'
 */
extern int getopt(int argc, char **argv, const char *opts);

/** setup optind and opterr */
extern void getopt_init(void);

#endif /* _GETOPT_H */
