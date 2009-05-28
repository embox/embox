#ifndef __CACHE_H__
#define __CACHE_H__

#include "types.h"
#include "leon.h"

inline static void cache_enable (BOOL enable)
{
    if (TRUE == enable)
        l_regs->cachectrl |= 0xF;
    else
        l_regs->cachectrl &= ~0xF;

    __asm__ ("flush");
}

inline static void cache_instr_enable (BOOL enable)
{
    if (TRUE == enable)
        l_regs->cachectrl |= 0x3;
    else
        l_regs->cachectrl &= ~0x3;

    __asm__ ("flush");
}


//data cache operations
inline static void cache_data_enable (BOOL enable)
{
    if (TRUE == enable)
        l_regs->cachectrl |= 0xC;
    else
        l_regs->cachectrl &= ~0xC;

    __asm__ ("flush");
}
#endif // ifndef __CACHE_H__
