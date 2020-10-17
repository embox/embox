/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.07.2014
 */

#ifndef GETOPT_H_
#define GETOPT_H_

#include <unistd.h>

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
		const char *optstring, const struct option *longopts, int *longindex);

#endif /* GETOPT_H_ */

