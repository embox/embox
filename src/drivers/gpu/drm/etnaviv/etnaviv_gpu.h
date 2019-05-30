/**
 * @file
 *
 * @date Jan 18, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_GPU_DRM_ETNAVIV_ETNAVIV_GPU_H_
#define SRC_DRIVERS_GPU_DRM_ETNAVIV_ETNAVIV_GPU_H_

#include <stdint.h>

#include <hal/reg.h>
#include <util/log.h>

#include "etnaviv_drv.h"
#include "etnaviv_mmu.h"

struct etnaviv_chip_identity {
	/* Chip model. */
	uint32_t model;

	/* Revision value.*/
	uint32_t revision;

	/* Supported feature fields. */
	uint32_t features;

	/* Supported minor feature fields. */
	uint32_t minor_features0;

	/* Supported minor feature 1 fields. */
	uint32_t minor_features1;

	/* Supported minor feature 2 fields. */
	uint32_t minor_features2;

	/* Supported minor feature 3 fields. */
	uint32_t minor_features3;

	/* Supported minor feature 4 fields. */
	uint32_t minor_features4;

	/* Supported minor feature 5 fields. */
	uint32_t minor_features5;

	/* Number of streams supported. */
	uint32_t stream_count;

	/* Total number of temporary registers per thread. */
	uint32_t register_max;

	/* Maximum number of threads. */
	uint32_t thread_count;

	/* Number of shader cores. */
	uint32_t shader_core_count;

	/* Size of the vertex cache. */
	uint32_t vertex_cache_size;

	/* Number of entries in the vertex output buffer. */
	uint32_t vertex_output_buffer_size;

	/* Number of pixel pipes. */
	uint32_t pixel_pipes;

	/* Number of instructions. */
	uint32_t instruction_count;

	/* Number of constants. */
	uint32_t num_constants;

	/* Buffer size */
	uint32_t buffer_size;

	/* Number of varyings */
	uint8_t varyings_count;
};

struct etnaviv_cmdbuf;
struct etnaviv_gpu {
	struct drm_device *drm;
	struct device *dev;

	struct etnaviv_chip_identity identity;
	void *mmio;

	/* 'ring'-buffer: */
	struct etnaviv_cmdbuf *buffer;

	uint32_t idle_mask;

	unsigned int freq_scale;
	struct etnaviv_cmdbuf_suballoc *cmdbuf_suballoc;

	struct etnaviv_iommu mmu;

	int exec_state;

	uint32_t memory_base;

	void *lastctx;
	int switch_context;

	/* Fencing support */
	uint32_t next_fence;
	uint32_t active_fence;
	uint32_t completed_fence;
	uint32_t retired_fence;

	int busy;
};

extern int etnaviv_gpu_get_param(struct etnaviv_gpu *gpu, uint32_t param,
		uint64_t *value);



static inline void gpu_write(struct etnaviv_gpu *gpu, uint32_t reg, uint32_t data)
{
	log_debug("gpu(%p) mmio(%p) + reg(%x): data(%x)", gpu, gpu->mmio, reg, data);
	REG32_STORE(gpu->mmio + reg, data);
}

static inline uint32_t gpu_read(struct etnaviv_gpu *gpu, uint32_t reg)
{
	uint32_t val = 0;
	log_debug("gpu(%p) mmio(%p) + reg(%x): val", gpu, gpu->mmio, reg, val);
	val = REG32_LOAD(gpu->mmio + reg);
	log_debug("gpu(%p) mmio(%p) + reg(%x): val", gpu, gpu->mmio, reg, val);
	return val;
}


static inline bool fence_completed(struct etnaviv_gpu *gpu, uint32_t fence)
{
	return fence_after_eq(gpu->completed_fence, fence);
}

static inline bool fence_retired(struct etnaviv_gpu *gpu, uint32_t fence)
{
	return fence_after_eq(gpu->retired_fence, fence);
}


extern int etnaviv_gpu_init(struct etnaviv_gpu *gpu);

extern void etnaviv_buffer_queue(struct etnaviv_gpu *gpu, unsigned int event,
	struct etnaviv_cmdbuf *cmdbuf);

extern void etnaviv_gpu_start_fe(struct etnaviv_gpu *gpu, uint32_t address, uint16_t prefetch);

extern int etnaviv_gpu_wait_idle(struct etnaviv_gpu *gpu, unsigned int timeout_ms);

extern int etnaviv_gpu_debugfs(struct etnaviv_gpu *gpu, char *s);

extern uint16_t etnaviv_buffer_init(struct etnaviv_gpu *gpu);

extern int etnaviv_gpu_wait_fence_interruptible(struct etnaviv_gpu *gpu,
		uint32_t fence, struct timespec *timeout);

#endif /* SRC_DRIVERS_GPU_DRM_ETNAVIV_ETNAVIV_GPU_H_ */
