/**
 * @file
 * @brief Convert a string to a double.
 *
 * @see stdlib.h
 *
 * @date 10.11.11
 * @author Nikolay Korotky
 * @author Alexander Lapshin
 */

#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

double strtod(const char *nptr, char **endptr) {
	double val= 0.0;
	int d = 0;
	int sign = 1;

	if(!nptr) {
		return 0.0;
	}

	if (*nptr == '+') {
		nptr++;
	}
	else if (*nptr == '-') {
		nptr++;
	    sign = -1;
	}

	while (*nptr >= '0' && *nptr <= '9') {
		val = val*10.0 + (*nptr - '0');
	    nptr++;
	}

	if (*nptr == '.') {
		nptr++;
	    while (*nptr >= '0' && *nptr <= '9') {
	    	val = val*10.0 + (*nptr - '0');
	        nptr++;
	        d--;
	    }
	 }

	 if (*nptr == 'E' || *nptr == 'e') {
		 int e_sign = 1;
	     int e_val = 0;

	     nptr++;
	     if (*nptr == '+') {
	    	 nptr++;
	     }
	     else if (*nptr == '-') {
	     	nptr++;
	         sign = -1;
	     }

	     while ((*nptr >= '0' && *nptr <= '9')) {
	    	 e_val = e_val*10 + (*nptr - '0');
	         nptr++;
	     }
	     d += e_val*e_sign;
	 }

	 while (d > 0) {
		 val *= 10.0;
	     d--;
	 }
	 while (d < 0) {
	     val *= 0.1;
	     d++;
	 }

	 if (endptr) {
		 *endptr = (char *)nptr;
	 }

	 return sign*val;
}

double atof(const char *nptr) {

	return strtod(nptr, (char **) NULL);

}
