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

    asm ("flush");
}

inline static void cache_instr_enable (BOOL enable)
{
    if (TRUE == enable)
        l_regs->cachectrl |= 0x3;
    else
        l_regs->cachectrl &= ~0x3;

    asm ("flush");
}


//data cache operations
inline static void cache_data_enable (BOOL enable)
{
    if (TRUE == enable)
        l_regs->cachectrl |= 0xC;
    else
        l_regs->cachectrl &= ~0xC;

    asm ("flush");
}
#endif // ifndef __CACHE_H__
