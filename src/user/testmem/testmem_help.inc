/**
 * \file testmem_help.inc
 * \date Jul 24, 2009
 * \author anton
 * \details
 */
"NAME\n"
"\ttestmem - set of memory tests\n"
"SYNOPSIS\n"
"\tmem [-h] [-a <addr>] [-c <count>] [-t <type of test]\n"
"DESCRIPTION\n"
"\tRead from memory or test memory.\n"
"OPTION\n"
"\t-h -  to see this page\n"
"\t-a addr\n"
"\t\t memory region start addr\n"
"\t-c count bytes\n"
"\t\t memory region size\n"
"\t-t - test type\n"
"\t\t runzero\n"
"\t\t runone\n"
"\t\t !n/a! address\n"
"\t\t !n/a! chess\n"
"\t\t loop\n"
"EXAMPLES\n"
"\ttestmem -a 0x40000000 -c 0x10000 -t loop\n"
"\ttestmem -a 0x40000000 -c 0x10000 -t runzero\n"
"SEE ALSO\n"
"\tmem,wmem\n"
"AUTHORS\n"
"Alexey Fomin\n"
