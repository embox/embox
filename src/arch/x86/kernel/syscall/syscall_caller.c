/**
 * @file
 * @brief
 *
 * @date 15.10.2012
 * @author Anton Bulychev
 */

#include <sys/syscall.h>

static int errno;

_syscall1(int,exit,int,errcode);
_syscall3(int,write,int,fd,const char *,buf,int,count);
