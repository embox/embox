/**
 * \file misc.c
 *
 * \date 27.05.09
 * \author sikmir
 */

#include "misc.h"

unsigned char *ipaddr_scan(unsigned char *addr, unsigned char res[4]) {
        unsigned int tmp;
        int i, j;
        for (i = 0; i < 3; i ++) {
                for (j = 0; j < 5; j ++) {
                        if (j > 4) {
                                return NULL;
                        }
                        if ('.' == addr[j]) {
                                addr [j] = 0;
                                if (1 != sscanf (addr, "%d", &tmp)) {
                                        return NULL;
                                }
                                if (tmp > 255) {
                                        return NULL;
                                }
                                res[i]=(unsigned char )0xFF & tmp;
                                addr += (j + 1);
                                break;
                        }
                }
        }
        if (1 != sscanf (addr, "%d", &tmp)) {
                return NULL;
        }
        if (tmp > 255) {
                return NULL;
        }
        res[3]=(unsigned char )0xFF & tmp;
        return  res;
}

unsigned char *macaddr_scan(unsigned char *addr, unsigned char res[6]) {
        unsigned int tmp;
        int i,j;
        for (i = 0; i < 5; i++) {
                for (j = 0; j < 4; j ++) {
                        if (j > 3) {
                                return NULL;
                        }
                        if ((':' == addr[j])||('-' == addr[j])) {
                                addr[j] = 0;
                                if (1 != sscanf(addr, "%x", &tmp)) {
                                        return 0;
                                }
                                if (tmp > 0xFF) {
                                        return NULL;
                                }
                                res[i] = (unsigned char) 0xFF & tmp;
                                addr += (j + 1);
                                break;
                        }
                }
        }
        if (1 != sscanf (addr, "%x", &tmp)) {
                return NULL;
        }
        if (tmp > 0xFF) {
                return NULL;
        }
        res[5]=(unsigned char )0xFF & tmp;
        return  res;
}

void ipaddr_print(char *buf, unsigned char *addr) {
        sprintf(buf, "%d.%d.%d.%d", addr[0], addr[1], addr[3], addr[3]);
}

void macaddr_print(char *buf, unsigned char *addr) {
        sprintf(buf, "%2X:%2X:%2X:%2X:%2X:%2X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}
