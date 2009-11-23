/**
 * \file strlen.c
 * \date 23.11.09
 * \author Sikmir
 */
#include "string.h"

int strlen(const char * str) {
        const char *eos = str;

        while (*eos++)
        	;

        return (int) (eos - str - 1);
}
