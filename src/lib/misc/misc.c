/**
 * \file misc.c
 *
 * \date 27.05.09
 * \author sikmir
 */

#include "misc.h"
#include "conio.h"
#include "string.h"

unsigned char *ipaddr_scan(unsigned char *addr, unsigned char res[4]) {
    unsigned char symbol_str[4];
    int i,j;
    int cur = 0;
    int tmp;
    for(i = 0; i < (sizeof(res) - 1); i ++){
        symbol_str[0]='\0';
        for (j = 0; j < array_len(symbol_str); j ++ ){
            if ('.' == addr[cur + j]){
                memcpy(symbol_str, &addr[cur], j );
                symbol_str[j] = '\0';
                break;
            }
        }
        if ('\0' == symbol_str[0]){
            return NULL;
        }
        if (1 != sscanf (symbol_str, "%d", &tmp)) {
            return NULL;
        }
        if (tmp > 0xFF)
            return NULL;
        res[i] = tmp;
        cur += j + 1;
    }
    strncpy(symbol_str, &addr[cur], array_len(symbol_str));
    if (1 != sscanf (symbol_str, "%d", &tmp)) {
        return NULL;
    }
    if (tmp > 0xFF)
        return NULL;
    res[i] = tmp;
    return res;
}

unsigned char *macaddr_scan(unsigned char *addr, unsigned char res[6]) {
    unsigned char symbol_str[4];
    int i,j;
    int cur = 0;
    int tmp;
    for(i = 0; i < 5; i ++){
        symbol_str[0]='\0';
        for (j = 0; j < array_len(symbol_str); j ++ ){
            if (':' == addr[cur + j]){
                memcpy(symbol_str, &addr[cur], j );
                symbol_str[j] = '\0';
                break;
            }
        }
        if ('\0' == symbol_str[0]){
            return NULL;
        }
        if (1 != sscanf (symbol_str, "%x", &tmp)) {
            return NULL;
        }
        if (tmp > 0xFF)
            return NULL;
        res[i] = tmp;
        cur += j + 1;
    }
    strncpy(symbol_str, &addr[cur], array_len(symbol_str));
    if (1 != sscanf (symbol_str, "%x", &tmp)) {
        return NULL;
    }
    if (tmp > 0xFF)
        return NULL;
    res[i] = tmp;
    return res;
}

void ipaddr_print(const char *buf, const unsigned char *addr) {
        sprintf((char *)buf, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
}

unsigned long inet_addr(const unsigned char *cp) {
        unsigned long tmp = 0x00000000;
        int i;
        for(i=0; i<4; i++) {
                tmp += ((0xFF & cp[i]) << (3-i)*8);
        }
        return tmp;
}

void macaddr_print(const char *buf, const unsigned char *addr) {
        sprintf((char *)buf, "%2X:%2X:%2X:%2X:%2X:%2X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}
