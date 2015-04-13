/**
 * @file
 * @brief See last debug message
 *
 * @date 14.03.12
 * @author Timur Abdukadyrov
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include <util/sys_log.h>

#define DEFAULT_COUNT 20

static void print_usage(void) {
	printf("Usage: log\t[-c cnt]\n\t\t[-h]\n\t\t[-i][-w][-e][-d]\n\t\t[-I]\n");
	printf("c\tmessage count\nh\tshow this message and quit\n");
	printf("i\tshow INFO messages\nw\tshow warning mwssages\n");
	printf("e\tshow error messages\nd\tshow debug messages\n");
	printf("I\ttoggle intrusive syslog mode(all messages are automatically\n");
	printf("displayed to stdout.\n");
	printf("When option -I and one or more of -w -e -i or -d options are set\n ");
	printf("then log just sets the types to output in intrusive mode.\n");
}

int main(int argc, char **argv) {
	int i, opt, cnt = DEFAULT_COUNT;
	bool disp_types[] = {true, true, true, true};
	/* indicates if types to show are inputed or defaults are used */
	bool setting_types = false;
	bool intrusive_mode = false;

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
			if (!setting_types){
				for(i=0; i<LT_MAX; i++)
					disp_types[i] = false;
			}
			if (opt == 'i')
				disp_types[LT_INFO] = true;
			if (opt == 'w')
				disp_types[LT_WARNING] = true;
			if (opt == 'e')
				disp_types[LT_ERROR] = true;
			if (opt == 'd')
				disp_types[LT_DEBUG] = true;
			setting_types = true;  /* we are setting types to display from cmd line  */
			break;
		case 'I':
			intrusive_mode = true;
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

	/* toggle intrusive mode and exit */
	if (intrusive_mode){
		if (setting_types){  /* if set then we are setting message types to display
													 for intrusive mode */
			syslog_toggle_intrusive(NULL);
		}
		printf("log:intrusive mode is %s\n", syslog_toggle_intrusive(disp_types)? "on" : "off");
		return 0;
	}

	/* show messages */
	show_log(cnt, disp_types);
	return 0;
}
