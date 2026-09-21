/**
 * @file
 * @brief QEMU ramfb framebuffer (fw_cfg "etc/ramfb")
 *
 * QEMU's -device ramfb turns a guest-owned buffer into the scanout:
 * the guest writes address, format and geometry into the fw_cfg file
 * etc/ramfb, and from then on the display simply shows that memory.
 *
 * @date 14.08.2026
 * @author Vitaliy Rybnikov
 */

#include <stdint.h>
#include <string.h>

#include <drivers/common/memory.h>
#include <drivers/video/fb.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/printk.h>
#include <mem/vmem.h>

#define FW_CFG_BASE     OPTION_GET(NUMBER, base_addr)
#define RAMFB_WIDTH     OPTION_GET(NUMBER, fb_xres)
#define RAMFB_HEIGHT    OPTION_GET(NUMBER, fb_yres)

#define RAMFB_BPP       32
#define RAMFB_STRIDE    (RAMFB_WIDTH * 4)
#define RAMFB_SIZE      (RAMFB_STRIDE * RAMFB_HEIGHT)

#define FW_CFG_DATA     (FW_CFG_BASE + 0x00)
#define FW_CFG_SELECTOR (FW_CFG_BASE + 0x08)
#define FW_CFG_DMA      (FW_CFG_BASE + 0x10)

#define FW_CFG_FILE_DIR 0x19

#define FW_CFG_DMA_CTL_ERROR  0x01u
#define FW_CFG_DMA_CTL_READ   0x02u
#define FW_CFG_DMA_CTL_SELECT 0x08u
#define FW_CFG_DMA_CTL_WRITE  0x10u

#define DRM_FORMAT_XRGB8888   0x34325258u

#define FW_CFG_FILES_MAX      64
#define FW_CFG_DMA_SPINS      100000

/* fw_cfg is big endian, the CPU is not */
struct fw_cfg_dma {
	uint32_t control;
	uint32_t length;
	uint64_t address;
} __attribute__((packed, aligned(8)));

struct ramfb_cfg {
	uint64_t addr;
	uint32_t fourcc;
	uint32_t flags;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
} __attribute__((packed, aligned(8)));

struct fw_cfg_file {
	uint32_t size;
	uint16_t select;
	uint16_t reserved;
	char name[56];
} __attribute__((packed));

static uint32_t ramfb_mem[RAMFB_WIDTH * RAMFB_HEIGHT]
    __attribute__((aligned(MMU_PAGE_SIZE)));

static int ramfb_set_var(struct fb_info *info,
    struct fb_var_screeninfo const *var) {
	(void)info;
	(void)var;
	return 0;
}

static int ramfb_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {
	(void)info;
	memset(var, 0, sizeof(*var));
	var->xres = var->xres_virtual = RAMFB_WIDTH;
	var->yres = var->yres_virtual = RAMFB_HEIGHT;
	var->bits_per_pixel = RAMFB_BPP;
	var->fmt = BGRA8888;
	return 0;
}

static struct fb_ops ramfb_ops = {
    .fb_set_var = ramfb_set_var,
    .fb_get_var = ramfb_get_var,
};

static void ramfb_dsb(void) {
	__asm__ volatile("dsb sy" ::: "memory");
}

static int ramfb_dma(uint16_t select, uint32_t ctl, void *buf, uint32_t len) {
	struct fw_cfg_dma dma;
	int i;

	dma.control = __builtin_bswap32(ctl | FW_CFG_DMA_CTL_SELECT
	                                | ((uint32_t)select << 16));
	dma.length = __builtin_bswap32(len);
	dma.address = __builtin_bswap64((uint64_t)(uintptr_t)buf);
	ramfb_dsb();
	MMAP_REG_STORE(uint64_t, FW_CFG_DMA,
	    __builtin_bswap64((uint64_t)(uintptr_t)&dma));
	ramfb_dsb();

	for (i = 0; i < FW_CFG_DMA_SPINS; i++) {
		uint32_t c = __builtin_bswap32(dma.control);

		if (c == 0) {
			return 0;
		}
		if (c & FW_CFG_DMA_CTL_ERROR) {
			return -1;
		}
	}
	return -1;
}

static int ramfb_find_file(const char *name, uint16_t *select_out,
    uint32_t *size_out) {
	uint8_t dir[4 + FW_CFG_FILES_MAX * sizeof(struct fw_cfg_file)];
	uint32_t count;
	uint32_t i;

	if (ramfb_dma(FW_CFG_FILE_DIR, FW_CFG_DMA_CTL_READ, dir, sizeof(dir)) < 0) {
		return -1;
	}
	memcpy(&count, dir, 4);
	count = __builtin_bswap32(count);
	if (count > FW_CFG_FILES_MAX) {
		count = FW_CFG_FILES_MAX;
	}
	for (i = 0; i < count; i++) {
		struct fw_cfg_file f;

		memcpy(&f, dir + 4 + i * sizeof(f), sizeof(f));
		if (strncmp(f.name, name, sizeof(f.name)) == 0) {
			*select_out = __builtin_bswap16(f.select);
			*size_out = __builtin_bswap32(f.size);
			return 0;
		}
	}
	return -1;
}

static int ramfb_init(void) {
	uint16_t sel;
	uint32_t sz;
	struct ramfb_cfg cfg;
	struct fb_info *info;

	if (ramfb_find_file("etc/ramfb", &sel, &sz) < 0) {
		printk("ramfb: etc/ramfb missing (start QEMU with -device ramfb)\n");
	}
	else {
		memset(&cfg, 0, sizeof(cfg));
		cfg.addr = __builtin_bswap64((uint64_t)(uintptr_t)ramfb_mem);
		cfg.fourcc = __builtin_bswap32(DRM_FORMAT_XRGB8888);
		cfg.flags = 0;
		cfg.width = __builtin_bswap32(RAMFB_WIDTH);
		cfg.height = __builtin_bswap32(RAMFB_HEIGHT);
		cfg.stride = __builtin_bswap32(RAMFB_STRIDE);
		if (sz > sizeof(cfg)) {
			sz = sizeof(cfg);
		}
		if (ramfb_dma(sel, FW_CFG_DMA_CTL_WRITE, &cfg, sz) < 0) {
			printk("ramfb: fw_cfg write failed\n");
		}
		else {
			printk("ramfb: scanout %ux%u @ %p\n", RAMFB_WIDTH, RAMFB_HEIGHT,
			    (void *)ramfb_mem);
		}
	}

	memset(ramfb_mem, 0, sizeof(ramfb_mem));

	info = fb_create(&ramfb_ops, (char *)ramfb_mem, RAMFB_SIZE);
	if (!info) {
		printk("ramfb: fb_create failed\n");
		return -1;
	}
	return 0;
}

EMBOX_UNIT_INIT(ramfb_init);
PERIPH_MEMORY_DEFINE(ramfb_fwcfg, FW_CFG_BASE, 0x18);
