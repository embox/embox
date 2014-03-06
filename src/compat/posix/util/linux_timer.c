/**
 * @file
 *
 * @date Jan 21, 2014
 * @author: Anton Bondarev
 */
#include <assert.h>
#include <sys/time.h>

#include <time.h>

void timeradd(struct timeval *a, struct timeval *b, struct timeval *result) {
	assert((a != NULL) && (b != NULL) && (result != NULL));
	result->tv_sec = a->tv_sec + b->tv_sec;
	result->tv_usec = a->tv_usec + b->tv_usec;
	if (result->tv_usec >= USEC_PER_SEC) {
		++result->tv_sec;
		result->tv_usec -= USEC_PER_SEC;
	}
}

void timersub(struct timeval *a, struct timeval *b, struct timeval *result) {
	assert((a != NULL) && (b != NULL) && (result != NULL));
	result->tv_sec = a->tv_sec - b->tv_sec;
	result->tv_usec = a->tv_usec - b->tv_usec;
	if (result->tv_usec < 0) {
		--result->tv_sec;
		result->tv_usec += USEC_PER_SEC;
	}
}

void timerclear(struct timeval *tvp) {
	assert(tvp != NULL);
	tvp->tv_sec = 0;
	tvp->tv_usec = 0;
}

int timerisset(struct timeval *tvp) {
	assert(tvp != NULL);
	return (tvp->tv_sec != 0) || (tvp->tv_usec != 0);
}
