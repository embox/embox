/**
 * @file
 * @brief OpenProm defines mainly taken from linux kernel header files.
 * @details Prom structures and defines for access to the OPENBOOT
 *              prom routines and data areas.
 *
 * @date 28.05.10
 * @author Nikolay Korotky
 */

#ifndef __SPARC_OPENPROM_H__
#define __SPARC_OPENPROM_H__

#ifndef __ASSEMBLY__

#include <open_prom.h>

#define M_LEON       0x30
#define M_LEON3_SOC  0x01
#define LEON_PREGS   0x80000000
#define LEON_SRLD    0x64

/* ram configuration */
#define LEONSETUP_MEM_BASEADDR 0x40000000
#define PAGE_OFFSET            0xf0000000

#define __pa(x)     ((unsigned long)(x) - PAGE_OFFSET + LEONSETUP_MEM_BASEADDR)
#define __va(x)     ((void *)((unsigned long) (x) - LEONSETUP_MEM_BASEADDR + PAGE_OFFSET))



/** ------------------- Leon 3 specific ----------------- */

struct property {
        char *name;
        char *value;
        int length;
};

struct node {
        int level;
        struct property *properties;
};

/**
 * a self contained prom info structure
 */
struct leon_prom_info {
        int freq_khz;
        int leon_nctx;
        int mids[32];
        int baudrates[2];
        struct property root_properties[4];
        struct property cpu_properties[7];
#undef  CPUENTRY
#define CPUENTRY(idx) struct property cpu_properties##idx[4];
        CPUENTRY(1)
        CPUENTRY(2)
        CPUENTRY(3)
        CPUENTRY(4)
        CPUENTRY(5)
        CPUENTRY(6)
        CPUENTRY(7)
        CPUENTRY(8)
        CPUENTRY(9)
        CPUENTRY(10)
        CPUENTRY(11)
        CPUENTRY(12)
        CPUENTRY(13)
        CPUENTRY(14)
        CPUENTRY(15)
        CPUENTRY(16)
        CPUENTRY(17)
        CPUENTRY(18)
        CPUENTRY(19)
        CPUENTRY(20)
        CPUENTRY(21)
        CPUENTRY(22)
        CPUENTRY(23)
        CPUENTRY(24)
        CPUENTRY(25)
        CPUENTRY(26)
        CPUENTRY(27)
        CPUENTRY(28)
        CPUENTRY(29)
        CPUENTRY(30)
        CPUENTRY(31)
        struct idprom idprom;
        struct linux_nodeops nodeops;
        struct linux_mlist_v0 *totphys_p;
        struct linux_mlist_v0 totphys;
        struct linux_mlist_v0 *avail_p;
        struct linux_mlist_v0 avail;
        struct linux_mlist_v0 *prommap_p;
        void (*synchook)(void);
        struct linux_arguments_v0 *bootargs_p;
        struct linux_arguments_v0 bootargs;
        struct linux_romvec romvec;
        struct node nodes[35];
        char s_device_type[12];
        char s_cpu[4];
        char s_mid[4];
        char s_idprom[7];
        char s_compatability[14];
        char s_leon2[6];
        char s_mmu_nctx[9];
        char s_frequency[16];
        char s_uart1_baud[11];
        char s_uart2_baud[11];
        char arg[];
};

extern struct leon_prom_info spi;

extern void mark(void);
extern void leon_prom_init(void);

#endif /* !(__ASSEMBLY__) */

#endif

