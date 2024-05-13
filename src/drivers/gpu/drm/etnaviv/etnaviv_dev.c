/**
 * @file
 * @brief Direct Rendering Manager compatibility module
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 06.12.2017
 */

#include <drm.h>
#include <errno.h>
#include <etnaviv_drm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <drivers/clk/ccm_imx6.h>
#include <drivers/common/memory.h>
#include <drivers/power/imx.h>
#include <embox_drm/drm_gem.h>
#include <embox_drm/drm_priv.h>
#include <etnaviv_xml/common.xml.h>
#include <etnaviv_xml/state_hi.xml.h>
#include <fs/dvfs.h>
#include <hal/cache.h>
#include <kernel/irq.h>
#include <mem/vmem.h>
#include <util/err.h>
#include <util/log.h>
#include <util/macro.h>

#include "etnaviv_drv.h"
#include "etnaviv_gem.h"
#include "etnaviv_gpu.h"

#define VERSION_NAME            "etnaviv"
#define VERSION_NAME_LEN        9
#define VERSION_DATE            "7 Dec 2017"
#define VERSION_DATE_LEN        10
#define VERSION_DESC            "DEADBEEF"
#define VERSION_DESC_LEN        8

/* Interrupt numbers */
#define GPU3D_IRQ               OPTION_GET(NUMBER, gpu3d_irq)
#define R2D_GPU2D_IRQ           OPTION_GET(NUMBER, r2d_gpu2d_irq)
#define V2D_GPU2D_IRQ           OPTION_GET(NUMBER, v2d_gpu2d_irq)

#define ETNA_UNCACHED_BUFFER_SZ (16 * 1024 * 1024)

static uint8_t etnaviv_uncached_buffer[ETNA_UNCACHED_BUFFER_SZ]
    __attribute__((aligned(0x1000)));
/*
 * DRM ioctls:
 */

static const char *drm_call_to_string[] = {
    [0x00] = "GET_PARAM   ",
    [0x01] = "SET_PARAM   ",
    [0x02] = "GEM_NEW     ",
    [0x03] = "GEM_INFO    ",
    [0x04] = "GEM_CPU_PREP",
    [0x05] = "GEM_CPU_FINI",
    [0x06] = "GEM_SUBMIT  ",
    [0x07] = "WAIT_FENCE  ",
    [0x08] = "GEM_USERPTR ",
    [0x09] = "GEM_WAIT    ",
    [0x0a] = "NUM_IOCTLS  ",
};

int etnaviv_ioctl_gem_new(struct drm_device *dev, void *data,
    struct drm_file *file) {
	struct drm_etnaviv_gem_new *args = data;

	if (args->flags
	    & ~(ETNA_BO_CACHED | ETNA_BO_WC | ETNA_BO_UNCACHED
	        | ETNA_BO_FORCE_MMU)) {
		log_error("unsupported flags");
	}

	return etnaviv_gem_new_handle(dev, file, args->size, args->flags,
	    (void *)&args->handle);
}

int etnaviv_ioctl_gem_info(struct drm_device *dev, void *data,
    struct drm_file *file) {
	struct drm_etnaviv_gem_info *args = data;
	struct drm_gem_object *obj;
	int ret;

	if (args->pad)
		return -EINVAL;

	obj = drm_gem_object_lookup(file, args->handle);
	if (!obj) {
		log_error("obj (%p) didn't found", args->handle);
		return -ENOENT;
	}
	log_debug("obj (%p) founded by handle (%i)", obj, args->handle);

	ret = etnaviv_gem_mmap_offset(obj, &args->offset);

	return ret;
}

static int etnaviv_ioctl_wait_fence(struct drm_device *dev, void *data,
    struct drm_file *file) {
	struct drm_etnaviv_wait_fence *args = data;
	struct etnaviv_drm_private *priv = dev->dev_private;
	struct etnaviv_gpu *gpu;

	if (args->flags & ~(ETNA_WAIT_NONBLOCK))
		return -EINVAL;

	if (args->pipe >= ETNA_MAX_PIPES)
		return -EINVAL;

	gpu = priv->gpu[args->pipe];
	if (!gpu)
		return -ENXIO;

	return 0;
}

static struct drm_device etnaviv_drm_device;
static struct drm_file etnaviv_drm_file;
static struct etnaviv_drm_private etnaviv_drm_private;
static struct etnaviv_gpu etnaviv_gpus[ETNA_MAX_PIPES];

#define VIVANTE_2D_BASE OPTION_GET(NUMBER, vivante_2d_base)
#define VIVANTE_3D_BASE OPTION_GET(NUMBER, vivante_3d_base)

static irq_return_t etna_irq_handler(unsigned int irq, void *data) {
	struct etnaviv_gpu *gpu = data;
	irq_return_t ret = IRQ_NONE;
	int noerr = 1;
	uint32_t intr = gpu_read(gpu, VIVS_HI_INTR_ACKNOWLEDGE);

	if (intr != 0) {
		log_debug("intr 0x%08x", intr);
		gpu->busy = 0;
		if (intr & VIVS_HI_INTR_ACKNOWLEDGE_AXI_BUS_ERROR) {
			uint32_t axi_status = gpu_read(gpu, VIVS_HI_AXI_STATUS);
			gpu_write(gpu, VIVS_HI_INTR_ACKNOWLEDGE,
			    VIVS_HI_INTR_ACKNOWLEDGE_AXI_BUS_ERROR);
			log_error("AXI bus error");
			log_debug("AXI config: %08x", gpu_read(gpu, VIVS_HI_AXI_CONFIG));
			log_debug("AXI status: %08x", axi_status);
			if (axi_status & VIVS_HI_AXI_STATUS_DET_WR_ERR) {
				log_error("AXI bus write error ID =0x%x",
				    VIVS_HI_AXI_STATUS_WR_ERR_ID(axi_status));
			}
			if (axi_status & VIVS_HI_AXI_STATUS_DET_RD_ERR) {
				log_error("AXI bus read error ID  =0x%x",
				    VIVS_HI_AXI_STATUS_RD_ERR_ID(axi_status));
			}
			noerr = 0;
		}

		if (intr & VIVS_HI_INTR_ACKNOWLEDGE_MMU_EXCEPTION) {
			int i;

			log_error("MMU fault status 0x%08x",
			    gpu_read(gpu, VIVS_MMUv2_STATUS));
			for (i = 0; i < 4; i++) {
				log_error("MMU %d fault addr 0x%08x\n", i,
				    gpu_read(gpu, VIVS_MMUv2_EXCEPTION_ADDR(i)));
			}

			gpu_write(gpu, VIVS_HI_INTR_ACKNOWLEDGE,
			    VIVS_HI_INTR_ACKNOWLEDGE_MMU_EXCEPTION);

			noerr = 0;
		}

		if (noerr) {
			log_debug("no error");
		}

		ret = IRQ_HANDLED;
	}

	return ret;
}

static int etnaviv_dev_open(struct char_dev *cdev, struct idesc *idesc) {
	int err;
	int i;

	etnaviv_drm_device.dev_private = &etnaviv_drm_private;
	for (i = 0; i < ETNA_MAX_PIPES; i++) {
		etnaviv_drm_private.gpu[i] = &etnaviv_gpus[i];
	}
	etnaviv_gpus[PIPE_ID_PIPE_2D].mmio = (void *)VIVANTE_2D_BASE;
	etnaviv_gpus[PIPE_ID_PIPE_3D].mmio = (void *)VIVANTE_3D_BASE;

	clk_enable("openvg");
	clk_enable("gpu3d");
	clk_enable("gpu2d");
	clk_enable("vpu");
	imx_gpu_power_set(1);
	etnaviv_gpu_init(&etnaviv_gpus[PIPE_ID_PIPE_2D]);
	etnaviv_gpu_debugfs(&etnaviv_gpus[PIPE_ID_PIPE_2D], "GPU2D");
	etnaviv_gpu_init(&etnaviv_gpus[PIPE_ID_PIPE_3D]);
	etnaviv_gpu_debugfs(&etnaviv_gpus[PIPE_ID_PIPE_2D], "GPU3D");

	if (irq_attach(GPU3D_IRQ, etna_irq_handler, 0,
	        &etnaviv_gpus[PIPE_ID_PIPE_3D], "i.MX6 GPU3D")) {
		return -1;
	}

	if (irq_attach(R2D_GPU2D_IRQ, etna_irq_handler, 0,
	        &etnaviv_gpus[PIPE_ID_PIPE_2D], "i.MX6 GPU2D")) {
		return -1;
	}

	if (irq_attach(V2D_GPU2D_IRQ, etna_irq_handler, 0,
	        &etnaviv_gpus[PIPE_ID_PIPE_2D], "i.MX6 GPU2D")) {
		return -1;
	}

	if ((err = vmem_set_flags(vmem_current_context(),
	         (mmu_vaddr_t)etnaviv_uncached_buffer,
	         sizeof(etnaviv_uncached_buffer),
	         PROT_WRITE | PROT_READ | PROT_NOCACHE))) {
		log_error("Failed to set page attributes! Error %d", err);

		return -1;
	}

	mmu_flush_tlb();
	dcache_flush(etnaviv_uncached_buffer, sizeof(etnaviv_uncached_buffer));

	return 0;
}

static void etnaviv_dev_close(struct char_dev *cdev) {
	if (irq_detach(GPU3D_IRQ, &etnaviv_gpus[PIPE_ID_PIPE_3D])) {
		log_error("Failed to detach GPU3D_IRQ");
	}

	if (irq_detach(R2D_GPU2D_IRQ, &etnaviv_gpus[PIPE_ID_PIPE_2D])) {
		log_error("Failed to detach R2D_GPU2D_IRQ");
	}

	if (irq_detach(V2D_GPU2D_IRQ, &etnaviv_gpus[PIPE_ID_PIPE_2D])) {
		log_error("Failed to detach V2D_GPU2D_IRQ");
	}

	imx_gpu_power_set(0);
}

int etnaviv_dmp(int id) {
	assert(id >= 0 && id < ETNA_MAX_PIPES);
	etnaviv_gpu_debugfs(&etnaviv_gpus[id],
	    id == PIPE_ID_PIPE_2D ? "GPU2D" : "GPU3D");
	return 0;
}

static int etnaviv_dev_idesc_ioctl(struct char_dev *cdev, int request,
    void *data) {
	drm_version_t *version;
	int nr = _IOC_NR(request);
	struct drm_device *dev = &etnaviv_drm_device;
	struct drm_file *file = &etnaviv_drm_file;
	struct drm_etnaviv_param *args = data;
	int res = 0;
	int pipe = args->pipe;

	log_debug("pipe=%cD, dir=%d, type=%d, nr=%d (%s)",
	    args->pipe == PIPE_ID_PIPE_2D ? '2' : '3', _IOC_DIR(request),
	    _IOC_TYPE(request), _IOC_NR(request),
	    (((nr - 0x40) < DRM_ETNAVIV_CALL_NR_MAX) && ((nr - 0x40) >= 0))
	        ? (drm_call_to_string[nr - 0x40])
	        : "UNKNOWN");
	switch (nr) {
	case 0: /* DRM_IOCTL_VERSION */
		version = data;
		*version = (drm_version_t){
		    .version_major = 1,
		    .version_minor = 1,
		    .name_len = VERSION_NAME_LEN,
		    .name = strdup(VERSION_NAME),
		    .date_len = VERSION_DATE_LEN,
		    .date = strdup(VERSION_DATE),
		    .desc_len = VERSION_DESC_LEN,
		    .desc = strdup(VERSION_DESC),
		};

		break;
	case DRM_COMMAND_BASE + DRM_ETNAVIV_GET_PARAM:
		res = etnaviv_gpu_get_param(&etnaviv_gpus[pipe], args->param,
		    &args->value);
		break;
	case DRM_COMMAND_BASE + DRM_ETNAVIV_GEM_NEW:
		res = etnaviv_ioctl_gem_new(dev, data, file);
		break;
	case DRM_COMMAND_BASE + DRM_ETNAVIV_GEM_INFO:
		res = etnaviv_ioctl_gem_info(dev, data, file);
		break;
	case DRM_COMMAND_BASE + DRM_ETNAVIV_GEM_SUBMIT:
		res = etnaviv_ioctl_gem_submit(dev, data, file);
		break;
	case DRM_COMMAND_BASE + DRM_ETNAVIV_WAIT_FENCE:
		res = etnaviv_ioctl_wait_fence(dev, data, file);
		break;
	default:
		log_error("NIY, request=%x", nr);
	}

	return res;
}

#if 0
static int etnaviv_dev_idesc_fstat(struct char_dev *cdev, void *buff) {
	struct stat *st = buff;

	st->st_rdev = makedev(226 /* Linux maj */, 0);
	st->st_mode = S_IFCHR;
	return 0;
}
#endif

static int etnaviv_dev_idesc_status(struct char_dev *cdev, int mask) {
	return 0;
}

#if 0
static int ptr = 0;
static void *etnaviv_dev_idesc_mmap(struct idesc *idesc, void *addr, size_t len,
    int prot, int flags, int fd, off_t off) {
	void *res = &etnaviv_uncached_buffer[ptr];
	struct drm_gem_object *obj;
	obj = (void *)(uint32_t)off;

	obj->dma_buf = res;

	ptr += len;

	memset(res, 0, len);

	return NULL;
}
#endif

static void *etnaviv_dev_direct_access(struct char_dev *cdev, off_t off,
    size_t len) {
	if (off + len >= sizeof(etnaviv_uncached_buffer)) {
		return NULL;
	}

	return &etnaviv_uncached_buffer[off];
}

static struct char_dev_ops etnaviv_dev_ops = {
    .open = etnaviv_dev_open,
    .close = etnaviv_dev_close,
    .ioctl = etnaviv_dev_idesc_ioctl,
    .status = etnaviv_dev_idesc_status,
    .direct_access = etnaviv_dev_direct_access,
};

static struct char_dev etnaviv_dev = CHAR_DEV_INIT(etnaviv_dev, "card",
    &etnaviv_dev_ops);

CHAR_DEV_REGISTER(&etnaviv_dev);

PERIPH_MEMORY_DEFINE(vivante2d, VIVANTE_2D_BASE, 0x4000);
PERIPH_MEMORY_DEFINE(vivante3d, VIVANTE_3D_BASE, 0x4000);
