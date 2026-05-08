/**
 * @file 
 * @brief
 *
 * @author  Ruslan Nafikov
 * @date    15.01.2026
 */

#ifndef LUA_STDIO_OVERRIDE_H_
#define LUA_STDIO_OVERRIDE_H_

/*
 * Override header for Embox third-party Lua build.
 * We want to keep using Embox stdio, but provide missing POSIX helpers.
 */
#include_next <stdio.h>

/* Embox libc may not provide these POSIX functions. Provide safe fallbacks. */
#ifndef flockfile
#define flockfile(f) ((void)0)
#endif

#ifndef funlockfile
#define funlockfile(f) ((void)0)
#endif

#ifndef getc_unlocked
#define getc_unlocked(f) getc(f)
#endif

#endif /* LUA_STDIO_OVERRIDE_H_ */
