/**
 * \file ping_help.inc
 *
 * \date 23.03.2009
 * \author abatyukov
 */
"*** PING\n"
" - test whether a particular host is reachable\n"
"*FORMAT: ping -i <eth_name> -d <destination> [-c <count>] [-W <timeout>] [-t <ttl>]\n"
"\t <eth_name>   - network interface's name.\n"
"\t              by default: eth0\n"
"\t <destination>- ip address of host.\n"
"\t <count>      - packets quantity.\n"
"\t              by default: 10 (-1 infinity)\n"
"\t <timeout>    - time to wait for a response\n"
"\t              by default: 1000"
