/**
 * @file
 * @brief
 *
 * @date 23.09.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <stddef.h>

int opterr;   // = 1;
int optind;   // = 1;
int optopt;   // = 0;
char *optarg; // = NULL;

int getopt_sp;      // = 1;
int getopt_not_opt; // = 0;

void getopt_init() {
	opterr = 1;
	optind = 1;
	optopt = 0;
	optarg = NULL;
	getopt_sp = 1;
	getopt_not_opt = 0;
}
