/**
 * \file misc.h
 *
 * \date 27.05.09
 * \author sikmir
 */

#ifndef MISC_H_
#define MIS_H_

#include "common.h"
#include "types.h"

unsigned char *ipaddr_scan(unsigned char *addr, unsigned char res[4]);
unsigned char *macaddr_scan(unsigned char *addr, unsigned char res[6]);
void ipaddr_print(char *buf, unsigned char *addr);
void macaddr_print(char *buf, unsigned char *addr);

#endif //MISC_H_
