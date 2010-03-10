/**
 * @file
 */
#include <shell_command.h>
#include <string.h>
#include <lib/inet/arpa/tftp.h>

#define COMMAND_NAME     "tftp"
#define COMMAND_DESC_MSG "TFTP client"
#define HELP_MSG         "Usage: tftp get ip filename size"
static const char *man_page =
	#include "tftp_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static int exec(int argsc, char **argsv) {
    int nextOption;
    char *cmd;
    getopt_init();
    do {
            nextOption = getopt(argsc, argsv, "c:h");
            switch(nextOption) {
	    case 'h':
    		    show_help();
                    return 0;
            case 'c':
        	    cmd = optarg;
        	    break;
            case -1:
                    break;
            default:
                    return 0;
            }
    } while(-1 != nextOption);

    char buffer[10000];
    if (!buffer) return 1;
    struct in_addr ip;
    inet_aton("192.168.0.59", &ip);
    int err = 0;
    tftp_client_get ("objs.lst", ip.s_addr, 69, buffer, 10000, TFTP_NETASCII, &err);
    return 0;
}
