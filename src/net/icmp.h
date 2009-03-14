/*
 * icmp.h
 *
 *  Created on: 14.03.2009
 *      Author: sunnya
 */

#ifndef ICMP_H_
#define ICMP_H_

typedef struct _icmphdr
{
	unsigned char type;
	unsigned char code;
	unsigned short header_check_summ;
	unsigned char *raw;
}icmphdr;

#endif /* ICMP_H_ */
