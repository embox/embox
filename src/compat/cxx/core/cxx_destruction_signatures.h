/*
 * cxx_destruction_signatures.h
 *
 *  Created on: 30 janv. 2013
 *      Author: fsulima
 */

#ifndef CXX_DESTRUCTION_SIGNATURES_H_
#define CXX_DESTRUCTION_SIGNATURES_H_

extern void *__dso_handle;
extern int __cxa_atexit(void (*destructor) (void *), void *arg, void *__dso_handle);
extern void __cxa_finalize(void *f);

#endif /* CXX_DESTRUCTION_SIGNATURES_H_ */
