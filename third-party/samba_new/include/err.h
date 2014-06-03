/*
 * err.h
 *
 *  Created on: 20 août 2013
 *      Author: fsulima
 */

#ifndef SAMBA_ERR_H_
#define SAMBA_ERR_H_

void err(int eval, const char *fmt, ...);
void errx(int eval, const char *fmt, ...);
void warn(const char *fmt, ...);
void warnx(const char *fmt, ...);

#endif /* SAMBA_ERR_H_ */
