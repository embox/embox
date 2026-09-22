/**
 * @file
 * @brief VideoCore mailbox framebuffer for BCM2711 (Raspberry Pi 4)
 *
 * The Pi 4 has no register-level display controller the kernel could
 * program: the framebuffer is set up by asking the VideoCore firmware
 * over the property mailbox (channel 8) to allocate one and report its
 * bus address, which is then mapped as uncached device memory and
 * published as /dev/fb0. The serial console is unaffected - HDMI is
 * scanout only.
 *
 * Unlike embox.driver.video.raspi_video this does one bulk SET +
 * ALLOCATE request, which is what the Pi 4 firmware expects, and takes
 * the mailbox base as an option instead of going through the
 * bcm2835_mailbox driver.
 *
 * @date 15.08.2026
 * @author Vitaliy Rybnikov
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <drivers/video/fb.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/cache.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <kernel/printk.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/mmap.h>
#include <mem/mmap.h>
#include <mem/page.h>
#include <mem/vmem.h>
#include <util/binalign.h>

#define MBOX_BASE ((uintptr_t)OPTION_GET(NUMBER, base_addr))
#define MBOX_READ (MBOX_BASE + 0x00)
#define MBOX_STATUS (MBOX_BASE + 0x18)
#define MBOX_WRITE (MBOX_BASE + 0x20)

#define MBOX_FULL 0x80000000u
#define MBOX_EMPTY 0x40000000u
#define MBOX_CHAN_TAGS 8u

#define MBOX_REQUEST 0u
#define MBOX_RESP_OK 0x80000000u
#define MBOX_TAG_END 0u

#define TAG_GETREV    0x10002u
#define TAG_GETARMMEM 0x10005u
#define TAG_GETVCMEM  0x10006u
#define TAG_GETDISP 0x40003u
#define TAG_SETDISP 0x48003u
#define TAG_SETVBUF 0x48004u
#define TAG_SETDEPTH 0x48005u
#define TAG_SETPIXORD 0x48006u
#define TAG_FALLOC 0x40001u
#define TAG_SETVIRTOFF 0x48009u
#define TAG_GETPITCH 0x40008u

#define FB_BPP        ((uint32_t)OPTION_GET(NUMBER, fb_bpp))
#define FB_FALLBACK_W ((uint32_t)OPTION_GET(NUMBER, fb_xres))
#define FB_FALLBACK_H ((uint32_t)OPTION_GET(NUMBER, fb_yres))

/* The firmware answers with a VideoCore bus address: bits 30-31 select the
 * cache alias. Strip the alias and keep the L2-coherent window, which is the
 * one that works on a physical board. */
#define VCADDR_TO_ARM(addr) (((addr) | 0x40000000u) & ~0xc0000000u)

static uint32_t s_xres;
static uint32_t s_yres;
static uint32_t s_bpp = FB_BPP;

static uint32_t mbox_load(uintptr_t addr) {
	uint32_t v;

	v = MMAP_REG_LOAD(uint32_t, addr);
	dmb(sy);
	return v;
}

static void mbox_store(uintptr_t addr, uint32_t val) {
	dmb(sy);
	MMAP_REG_STORE(uint32_t, addr, val);
}

static int mbox_send(uint32_t *buf, size_t bytes) {
	uint32_t msg;
	uint32_t got;
	uint32_t spins;

	msg = (uint32_t)(uintptr_t)buf;
	if (msg & 0xfu) {
		printk("bcm2711_fb: mailbox buffer not 16-aligned\n");
		return -EINVAL;
	}

	dcache_flush(buf, bytes);
	dsb(sy);

	spins = 0;
	while (mbox_load(MBOX_STATUS) & MBOX_FULL) {
		if (++spins > 10000000u) {
			printk("bcm2711_fb: mailbox write timeout\n");
			return -ETIMEDOUT;
		}
	}
	mbox_store(MBOX_WRITE, msg | MBOX_CHAN_TAGS);

	spins = 0;
	for (;;) {
		while (mbox_load(MBOX_STATUS) & MBOX_EMPTY) {
			if (++spins > 10000000u) {
				printk("bcm2711_fb: mailbox read timeout\n");
				return -ETIMEDOUT;
			}
		}
		got = mbox_load(MBOX_READ);
		if ((got & 0xfu) == MBOX_CHAN_TAGS) {
			break;
		}
	}

	dcache_inval(buf, bytes);
	dsb(sy);

	if ((got & ~0xfu) != msg) {
		printk("bcm2711_fb: mailbox reply %08x want %08x\n", got, msg);
		return -EIO;
	}
	if (buf[1] != MBOX_RESP_OK) {
		printk("bcm2711_fb: mailbox code %08x\n", buf[1]);
		return -EIO;
	}
	return 0;
}

/* The revision word encodes the model, the stepping and the RAM size; the
 * two memory tags say how much DRAM the ARM was given and where the
 * VideoCore kept its own, which is what the RAM region has to stay clear of.
 * The linker map is pinned to one SKU, so a board swap changes all three
 * without changing anything in the tree. A tag the firmware does not know is
 * skipped in silence. */
static void mbox_report_board(void) {
	uint32_t buf[8] __attribute__((aligned(16)));
	uint32_t rev;
	unsigned mib;

	memset(buf, 0, sizeof(buf));
	buf[0] = sizeof(buf);
	buf[1] = MBOX_REQUEST;
	buf[2] = TAG_GETREV;
	buf[3] = 4;
	buf[4] = 0;
	buf[6] = MBOX_TAG_END;
	if (mbox_send(buf, sizeof(buf)) == 0) {
		rev = buf[5];
		/* New-style revision: bits 20-22 are 256 MiB << code. Old-style
		 * codes do not set bit 23. */
		mib = (rev & (1u << 23)) ? (256u << ((rev >> 20) & 7u)) : 0u;
		if (mib) {
			printk("bcm2711_fb: board revision %08x, %u MiB\n", rev, mib);
		}
		else {
			printk("bcm2711_fb: board revision %08x\n", rev);
		}
	}

	memset(buf, 0, sizeof(buf));
	buf[0] = sizeof(buf);
	buf[1] = MBOX_REQUEST;
	buf[2] = TAG_GETARMMEM;
	buf[3] = 8;
	buf[4] = 0;
	buf[7] = MBOX_TAG_END;
	if (mbox_send(buf, sizeof(buf)) == 0) {
		printk("bcm2711_fb: ARM memory %08x + %08x (%u MiB)\n", buf[5], buf[6],
				buf[6] >> 20);
	}

	memset(buf, 0, sizeof(buf));
	buf[0] = sizeof(buf);
	buf[1] = MBOX_REQUEST;
	buf[2] = TAG_GETVCMEM;
	buf[3] = 8;
	buf[4] = 0;
	buf[7] = MBOX_TAG_END;
	if (mbox_send(buf, sizeof(buf)) == 0) {
		printk("bcm2711_fb: VC memory  %08x + %08x (%u MiB)\n", buf[5], buf[6],
				buf[6] >> 20);
	}
}

static int mbox_get_disp(uint32_t *x, uint32_t *y) {
	uint32_t buf[8] __attribute__((aligned(16)));

	memset(buf, 0, sizeof(buf));
	buf[0] = sizeof(buf);
	buf[1] = MBOX_REQUEST;
	buf[2] = TAG_GETDISP;
	buf[3] = 8;
	buf[4] = 0;
	buf[7] = MBOX_TAG_END;
	if (mbox_send(buf, sizeof(buf)) < 0) {
		return -EIO;
	}
	*x = buf[5];
	*y = buf[6];
	return 0;
}

static int mbox_fbinit(uint32_t *x, uint32_t *y, uint32_t *bpp, void **fb,
		uint32_t *fblen, uint32_t *pitch) {
	uint32_t buf[35] __attribute__((aligned(16)));

	memset(buf, 0, sizeof(buf));
	buf[0] = sizeof(buf);
	buf[1] = MBOX_REQUEST;

	buf[2] = TAG_SETDISP;
	buf[3] = 8;
	buf[4] = 0;
	buf[5] = *x;
	buf[6] = *y;

	buf[7] = TAG_SETVBUF;
	buf[8] = 8;
	buf[9] = 8;
	buf[10] = *x;
	buf[11] = *y;

	buf[12] = TAG_SETVIRTOFF;
	buf[13] = 8;
	buf[14] = 8;
	buf[15] = 0;
	buf[16] = 0;

	buf[17] = TAG_SETDEPTH;
	buf[18] = 4;
	buf[19] = 4;
	buf[20] = *bpp;

	/* 0 = "BGR" in the firmware docs, i.e. BGRA8888 in memory */
	buf[21] = TAG_SETPIXORD;
	buf[22] = 4;
	buf[23] = 4;
	buf[24] = 0;

	buf[25] = TAG_FALLOC;
	buf[26] = 8;
	buf[27] = 8;
	buf[28] = *bpp;
	buf[29] = 0;

	buf[30] = TAG_GETPITCH;
	buf[31] = 4;
	buf[32] = 4;
	buf[33] = 0;

	buf[34] = MBOX_TAG_END;

	if (mbox_send(buf, sizeof(buf)) < 0) {
		return -EIO;
	}

	*x = buf[10];
	*y = buf[11];
	*bpp = buf[20];
	*fb = (void *)(uintptr_t)VCADDR_TO_ARM(buf[28]);
	*fblen = buf[29];
	*pitch = buf[33];
	return 0;
}

static int bcm2711_fb_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	(void)info;
	(void)var;
	return 0;
}

static int bcm2711_fb_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {
	(void)info;
	memset(var, 0, sizeof(*var));
	var->xres = var->xres_virtual = s_xres;
	var->yres = var->yres_virtual = s_yres;
	var->bits_per_pixel = s_bpp;
	var->fmt = BGRA8888;
	return 0;
}

static struct fb_ops bcm2711_fb_ops = {
	.fb_set_var = bcm2711_fb_set_var,
	.fb_get_var = bcm2711_fb_get_var,
};

static int bcm2711_fb_init(void) {
	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t bpp = FB_BPP;
	uint32_t fblen = 0;
	uint32_t pitch = 0;
	void *fb = NULL;
	struct fb_info *info;
	uint32_t *px;
	unsigned n;
	int tries;

	mbox_report_board();

	if (mbox_get_disp(&x, &y) < 0 || x == 0 || y == 0) {
		x = FB_FALLBACK_W;
		y = FB_FALLBACK_H;
		printk("bcm2711_fb: no EDID size, using %ux%u\n", x, y);
	}

	tries = 0;
	do {
		fb = NULL;
		fblen = 0;
		pitch = 0;
		bpp = FB_BPP;
		if (mbox_fbinit(&x, &y, &bpp, &fb, &fblen, &pitch) == 0 && fb && fblen) {
			break;
		}
		tries++;
	} while (tries < 3);

	if (!fb || !fblen) {
		printk("bcm2711_fb: allocate failed (HDMI unplugged?)\n");
		return 0;
	}
	if (bpp != FB_BPP) {
		printk("bcm2711_fb: got %u bpp, want %u\n", bpp, FB_BPP);
		return 0;
	}

	s_xres = x;
	s_yres = y;
	s_bpp = bpp;

	{
		uintptr_t start = (uintptr_t)fb & ~(uintptr_t)MMU_PAGE_MASK;
		uintptr_t map_len = binalign_bound((uintptr_t)fb + fblen, MMU_PAGE_SIZE)
				- start;
		int prot = PROT_READ | PROT_WRITE | PROT_NOCACHE;

		/* GPU DRAM sits above ARM RAM in lds.conf. Do not switch the
		 * image to vmem_device_memory_full: that would change phymem_init. */
		(void)mmap_place(task_resource_mmap(task_kernel_task()), start, map_len,
				prot);
		if (vmem_map_region(vmem_current_context(), start, start, map_len, prot)) {
			printk("bcm2711_fb: vmem_map_region %p failed\n", fb);
			return 0;
		}
	}

	px = fb;
	n = fblen / 4u;
	while (n--) {
		*px++ = 0xff202020u;
	}

	info = fb_create(&bcm2711_fb_ops, (char *)fb, binalign_bound(fblen, PAGE_SIZE()));
	if (!info) {
		printk("bcm2711_fb: fb_create failed\n");
		return 0;
	}

	printk("bcm2711_fb: /dev/fb0 %ux%u pitch=%u @ %p (%u bytes)\n", x, y, pitch, fb,
			fblen);
	if (pitch && pitch != x * (bpp / 8u)) {
		printk("bcm2711_fb: pitch %u != xres*bpp/8 %u\n", pitch,
				x * (bpp / 8u));
	}
	return 0;
}

EMBOX_UNIT_INIT(bcm2711_fb_init);
PERIPH_MEMORY_DEFINE(bcm2711_mbox, MBOX_BASE, 0x40);
