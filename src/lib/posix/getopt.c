/**
 * \file getopt.c
 * \date 23.09.09
 * \author Sikmir
 */
#include "getopt.h"
#include "common.h"
#include "conio.h"
#include "string.h"

/*int parse_arg(const char *handler_name, int argsc, char **argsv,
		char *available_keys, int amount_of_available_keys, SHELL_KEY *keys) {
	int i, j, args_count;

	i = 1;
	argsc--;
	args_count = 0;

	while (i < argsc) {
		if (*argsv[i] != '-') {
			LOG_ERROR("%s: wrong condition found. Arguments not in format: -<key> [<value>]\n",
					handler_name);

			return -1;
		}

		if (args_count >= MAX_SHELL_KEYS) {
			LOG_ERROR("%s: wrong key entered. See help.\n", handler_name);
			return -2;
		}
		// Get key name.
		// Second symbol. Ex: -h -x (-hello == -h)
		keys[args_count].name = *(*(argsv + i) + 1);
		keys[args_count].value = 0x0;

		// Now working with value if one exists
		i++;
		if (i < argsc && **(argsv + i) != '-') {
			keys[args_count].value = *(argsv + i);
			i++;
		}
		args_count++;
	}

	//return args_count;

	for (i = 0; i < args_count; i++) {
		for (j = 0; j < amount_of_available_keys; j++) {
			if (keys[i].name == available_keys[j]) {
				break;
			}
		}
		if (j >= amount_of_available_keys) {
			LOG_ERROR("%s: incorrect key entered! See help.\n",
					handler_name);
			return -3;
		}
	}
	return args_count;
}*/

/*int check_keys(SHELL_KEY *keys, int amount, char *available_keys,
        int amount_of_available_keys) {
        int i, j;

        for (i = 0; i < amount; i++) {
                for (j = 0; j < amount_of_available_keys; j++) {
                        if (keys[i].name == available_keys[j]) {
                                break;
                        }
                }
                if (j >= amount_of_available_keys) {
                        return FALSE;
                }
        }
        return TRUE;
}*/

/*int get_key(char key, SHELL_KEY *keys, int keys_amount, char **value) {
        int i;
        for (i = 0; i < keys_amount; i++) {
                if (keys[i].name == key) {
                	*value = keys[i].value;
                        return TRUE;
                }
        }
        return FALSE;
}*/

int   opterr = 1;   /* flag:error message on unrecognzed options */
int   optind = 1;   /* last touched cmdline argument */
int   optopt;       /* last returned option */
char  *optarg;      /* argument to optopt */

int getopt(int argc, char **argv, const char *opts) {
	static int sp = 1;
	int c;
	char *cp;
	if(sp == 1)
		/* check for end of options */
	        if(optind >= argc ||
	    	    (argv[optind][0] != '/' &&
	    	    argv[optind][0] != '-') ||
	            argv[optind][1] == '\0')
	        	return -1;
	        else if(!strcmp(argv[optind], "--")) {
	    	    optind++;
	            return -1;
	        }
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == NULL) {
	        /* if arg sentinel as option or other invalid option,
	          handle the error and return '?' */
	        LOG_ERROR("illegal option -- ", (char)c);
	        if(argv[optind][++sp] == '\0') {
	                optind++;
	                sp = 1;
	        }
	        return '?';
	}
	if(*++cp == ':') {
	        /* if option is given an argument...  */
	        if(argv[optind][sp+1] != '\0')
	    		/* and the OptArg is in that CmdLineArg, return it... */
	                optarg = &argv[optind++][sp+1];
	        else if(++optind >= argc) {
	                /* but if the OptArg isn't there and the next CmdLineArg
	                   isn't either, handle the error... */
	                LOG_ERROR(": option requires an argument -- ", (char)c);
	    		sp = 1;
	    		return '?';
		} else
	    		/* but if there is another CmdLineArg there, return that */
	    		optarg = argv[optind++];
		/* and set up for the next CmdLineArg */
		sp = 1;
	} else {
	        /* no arg for this opt, so null arg and set up for next option */
		if(argv[optind][++sp] == '\0') {
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
