/**
 * @file etnaviv_compat.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 31.01.2018
 */

#ifndef ETNAVIV_COMPAT_H_
#define ETNAVIV_COMPAT_H_

#include <errno.h>
#include <linux/list.h>
#include <linux/sizes.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <mem/sysmalloc.h>
#include <string.h>
#include <stdbool.h>

#define GFP_TEMORARY  0
#define GFP_KERNEL    0
#define __GFP_NOWARN  0
#define __GFP_NORETRY 0

#define drm_malloc_ab(a, b) sysmalloc((a) * (b))
#define kmalloc(a, b) sysmalloc(a)

static inline void *kzalloc(int a, int b) {
	void *m = sysmalloc(a);
	memset(m, 0, a);
	return m;
}

#define kfree(a) sysfree(a)

#define ALIGN(v,a) (((v) + (a) - 1) & ~((a) - 1))
#define u64_to_user_ptr(a) ((uint32_t) a)
#define copy_from_user(a, b, c) (0 * (int) memcpy((void*) a, (void*) b, c))

#define DRM_ERROR log_error

struct reservation_object {
	int stub;
};

struct vm_area_struct {
	int stub;
};

struct ww_acquire_ctx {
	int stub;
};

static inline int order_base_2(int q) {
	int res;
	while (q > 0) {
		res++;
		q /= 2;
	}

	return res;
};

#define BIT(n) (1 << (n))

struct platform_device;
struct drm_device;
struct drm_file;
struct file_desc;
struct drm_gem_object;
struct timespec;

#define PHYS_OFFSET 0x10000000 /* Start of RAM */

static inline int dma_get_required_mask(void *dev) {
	return 0x1FFFFFFF; /* 512 MB */
}

#define IOMMU_READ	(1 << 0)

int etnaviv_dmp(int id); /* Print debug information about GPU device */

#define DRM_ETNAVIV_CALL_NR_MAX        DRM_ETNAVIV_NUM_IOCTLS

#endif
