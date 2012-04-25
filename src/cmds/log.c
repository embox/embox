/**
 * @file
 * @brief Ping hosts by ARP requests/replies.
 *
 * @date 23.12.09
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include <util/sys_log.h>

#define DEFAULT_COUNT 20

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: log\t[-c cnt]\n\t\t[-h]\n\t\t[-i][-w][-e][-d]\n[-I]");
	printf("c\tmessage count\nh\tshow this message and quit\n");
	printf("i\tshow INFO messages\nw\tshow warning mwssages\n");
	printf("e\tshow error messages\nd\tshow debug messages\n");
	printf("I\ttoggle intrusive syslog mode(all messages are automatically\n");
	printf("displayed to stdout.\n");
}

static int exec(int argc, char **argv) {
	int i, opt, cnt = DEFAULT_COUNT;
	bool disp_types[] = {true, true, true, true};
	/* indicates if types to show are inputed or defaults are used */
	bool setting_types = false;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "c:h:i w e d I"))) {
		switch (opt) {
		case 'c': /* get msg cnt */
			if (1 != sscanf(optarg, "%d", &cnt) || !cnt){
				printf("log: bad message count. using default\n");
				cnt = DEFAULT_COUNT;
			}
			break;
		case 'i':
		case 'w':
		case 'e':
		case 'd':
			/* if types to display are inputed from command line */
			if(!setting_types){
				for(i=0; i<LT_MAX; i++)
					disp_types[i] = false;
			}
			if(opt == 'i')
				disp_types[LT_INFO] = true;
			if(opt == 'w')
				disp_types[LT_WARNING] = true;
			if(opt == 'e')
				disp_types[LT_ERROR] = true;
			if(opt == 'd')
				disp_types[LT_DEBUG] = true;
			setting_types = true;  /* we are setting types to display from cmd line  */
			break;
		case 'I':
			printf("log:intrusive mode is %s\n", syslog_toggle_intrusive()? "on" : "off");
			return 0;
			break;
		case '?':
			printf("Invalid option `-%c'\n", optopt);
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	};

	show_log(cnt, disp_types);

	return 0;
}
