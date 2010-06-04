/**
 * @file
 * @brief Procedures for creating, accessing and interpreting the device tree.
 * @details emulates a sparc v0 PROM for the linux kernel.
 *
 * @date 27.05.10
 * @author Nikolay Korotky
 */

#include <asm/prom.h>
#include <hal/mm/mmu_core.h>
#include <hal/mm/mmu_page.h>
#include <kernel/printk.h>
#include <string.h>

#define KERNEL_ARGS "console=ttyS0,38400 rdinit=/sbin/init"

static void leon_reboot(char *bcommand);
static void leon_halt(void);
static int leon_nbputchar(int c);
static int leon_nbgetchar(void);

static int no_nextnode(int node);
static int no_child(int node);
static int no_proplen(int node,char *name);
static int no_getprop(int node,char *name,char *value);
static int no_setprop(int node,char *name,char *value,int len);
static char *no_nextprop(int node,char *name);

/* static prom info */
struct leon_prom_info spi = {
        0, /* freq_khz */
        256, /* leon_nctx */
        {
#undef  CPUENTRY
#define CPUENTRY(idx)  idx,
                CPUENTRY(0)
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
                31
        },
        { /* baudrates */
                38400, 38400
        },
        { /* root_properties */
                {__va(spi.s_device_type), (char*)__va(spi.s_idprom), 4},
                {__va(spi.s_idprom), (char *)__va(&spi.idprom), sizeof(struct idprom)},
                {__va(spi.s_compatability), __va(spi.s_leon2), 5},
                {NULL, NULL, -1}
        },
        { /* cpu_properties */
                {__va(spi.s_device_type), __va(spi.s_cpu), 4},
                {__va(spi.s_mid), (char*)__va(&spi.mids[0]), 4},
                {__va(spi.s_mmu_nctx), (char *)__va(&spi.leon_nctx), 4},
                {__va(spi.s_frequency), (char *)__va(&spi.freq_khz), 4},
                {__va(spi.s_uart1_baud), (char *)__va(&spi.baudrates[0]), 4},
                {__va(spi.s_uart2_baud), (char *)__va(&spi.baudrates[1]), 4},
                {NULL, NULL, -1}
        },
#undef  CPUENTRY
#define CPUENTRY(idx)                                                   \
        { /* cpu_properties */                                          \
          {__va(spi.s_device_type), (char*)__va(spi.s_cpu), 4},         \
          {__va(spi.s_mid), (char*)__va(&spi.mids[idx]), 4},            \
          {__va(spi.s_frequency), (char *)__va(&spi.freq_khz), 4},      \
          {NULL, NULL, -1}                                              \
        },

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

        { /* idprom */
                0x01, /* format */
                M_LEON | M_LEON3_SOC /* maybe 0x01? */, /* machine type */
                {0,0,0,0,0,0}, /* eth */
                0, /* date */
                0, /* sernum */
                0, /* checksum */
                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* reserved */
        },
        { /* nodeops */
                __va(no_nextnode),
                __va(no_child),
                __va(no_proplen),
                __va(no_getprop),
                __va(no_setprop),
                __va(no_nextprop)
        },
        __va(&spi.totphys), /* totphys_p */
        { /* totphys */
                NULL,
                (char *)LEONSETUP_MEM_BASEADDR,
                0,
        },
        __va(&spi.avail), /* avail_p */
        { /* avail */
                NULL,
                (char *)LEONSETUP_MEM_BASEADDR,
                0,
        },
        NULL, /* prommap_p */
	NULL, /* synchook  */
        __va(&spi.bootargs), /* bootargs_p */
        { /* bootargs */
                { NULL, __va(spi.arg), NULL /*...*/
    		},
    		/*...*/
        },
        { /* romvec */
                0,
                0, /* sun4c v0 prom */
                0, 0,
                { __va(&spi.totphys_p), __va(&spi.prommap_p), __va(&spi.avail_p) },
                __va(&spi.nodeops),
                NULL, { NULL /* ... */
                      },
                NULL, NULL,
                NULL, NULL,  /* pv_getchar, pv_putchar */
                __va(leon_nbgetchar), __va(leon_nbputchar),
                NULL,
                __va(leon_reboot),
                NULL,
                NULL,
                NULL,
                __va(leon_halt),
                __va(&spi.synchook),
                { NULL },
                __va(&spi.bootargs_p)
                /*...*/
        },
        { /* nodes */
                { 0, __va(spi.root_properties+3) /* NULL, NULL, -1 */
                },
                { 0, __va(spi.root_properties) },
                { 1, __va(spi.cpu_properties) }, /* cpu 0, must be spi.nodes[2] see leon_prom_init()*/

#undef  CPUENTRY
#define CPUENTRY(idx) \
          { 0, __va(spi.cpu_properties##idx) }, /* cpu <idx> */
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
        { -1,__va(spi.root_properties+3) /* NULL, NULL, -1 */
        }
        },
        "device_type",
        "cpu",
        "mid",
        "idprom",
        "compatability",
        "leon2",
        "mmu-nctx",
        "clock-frequency",
        "uart1_baud",
        "uart2_baud",
        KERNEL_ARGS
};

static void leon_reboot(char *bcommand) {
        while (1) {
                printk(__va("Can't reboot\n"));
        };
}

static void leon_halt(void) {
        while (1) {
                printk(__va("Halt\n"));
        };
}

/* get single char, don't care for blocking*/
static int leon_nbgetchar(void) {
        return -1;
}

/* put single char, don't care for blocking*/
static int leon_nbputchar(int c) {
#ifdef CONFIG_LEON_3
# ifdef CONFIG_GRLIB_GAISLER_APBUART
//        extern void leon3_rs_put_char(char ch);
//        leon3_rs_put_char(c);
# endif
#else
///        unsigned int old_cr;

//        old_cr = LEON_REGLOAD_PA(LEON_UCTRL0);
//        LEON_REGSTORE_PA(LEON_UCTRL0,(old_cr & ~(LEON_UCTRL_TI)) | (LEON_UCTRL_TE));

//        while (!(LEON_REGLOAD_PA(LEON_USTAT0) & 0x4));
//        LEON_REGSTORE_PA(LEON_UDATA0, (unsigned char)c);
//        while (!(LEON_REGLOAD_PA(LEON_USTAT0) & 0x4));
//        LEON_REGSTORE_PA(LEON_UCTRL0,old_cr);
#endif
        return 0;
}

/* node ops */

#define bnodes ((struct node *)__va(&spi.nodes))

static int no_nextnode(int node) {
        if (bnodes[node].level == bnodes[node+1].level)
                return node+1;
        return -1;
}

static int no_child(int node) {
        if (bnodes[node].level == bnodes[node+1].level-1)
                return node+1;
        return -1;
}

static struct property *find_property(int node,char *name) {
        struct property *prop = &bnodes[node].properties[0];
        while (prop && prop->name) {
        	if (strcmp(prop->name,name) == 0) return prop;
                        prop++;
        }
        return NULL;
}

static int no_proplen(int node,char *name) {
        struct property *prop = find_property(node,name);
        if (prop) return prop->length;
        return -1;
}

static int no_getprop(int node,char *name,char *value) {
        struct property *prop = find_property(node,name);
        if (prop) {
                memcpy(value,prop->value,prop->length);
                return 1;
        }
        return -1;
}

static int no_setprop(int node,char *name,char *value,int len) {
        return -1;
}

static char *no_nextprop(int node,char *name) {
        struct property *prop;
        if (!name || !name[0] ) {
                return bnodes[node].properties[0].name;
        }
        prop = find_property(node,name);
        return (prop != NULL) ? prop[1].name : NULL;
}

void mark(void) {
        __asm__ __volatile__(
                "sethi  %%hi(%0), %%l0    \n\t" \
                "st    %%g0,[%%lo(%0)+%%l0]\n\t" \
        : : "i" (LEONSETUP_MEM_BASEADDR) : "l0" );
}

static inline void set_cache(unsigned long regval) {
        asm volatile("sta %0, [%%g0] %1\n\t" : :
                     "r" (regval), "i" (2) : "memory");
}

#define GETREGSP(sp) __asm__ __volatile__("mov %%sp, %0" : "=r" (sp))

void leon_prom_init(void) {
        unsigned long i;
        unsigned char cksum, *ptr;

        unsigned long sp;
        GETREGSP(sp);

        /*
         * the frequency is actually detected using
         * (LEON_REGLOAD_PA(LEON_SRLD)+1) * 1000 * 1000; assuming that
         * bootloader or the dsumon has set up LEON_SRLD, so that timer ticks
         * on 1mhz
         */
        spi.freq_khz = (*((unsigned long*)(LEON_PREGS+LEON_SRLD))+1) * 1000;

        spi.totphys.num_bytes = 0;

        sp = sp - LEONSETUP_MEM_BASEADDR;
        spi.totphys.num_bytes = (sp + 0x1000) & ~(0xfff);

        spi.avail.num_bytes = spi.totphys.num_bytes;

        ptr = (unsigned char *)&spi.idprom;
        for (i = cksum = 0; i <= 0x0E; i++)
                cksum ^= *ptr++;
        spi.idprom.id_cksum = cksum;
}
