/*
 * mem_help.inc
 *
 *  Created on: 16.02.2009
 *      Author: Alexey Fomin
 */
"*** MEM\n"
" - read from memory or test memory\n"
"* FORMAT: mem [-h] [-a <addr>] [-c <count>] [-t]\n"
"\t -h ------ to see this page\n"
"\t <addr> -- address of first element to be read\n"
"\t			in read mode - 0x0 			if not presented\n"
"\t			in test mode - 0x70000000   if not presented\n"
"\t <count> - amount of elements to be read\n"
"\t			in read mode - 50 		 	if not presented\n"
"\t			in test mode - 0x1000000 	if not presented\n"
"\t -t ------ test mode (try to write memory in the same range)\n"

