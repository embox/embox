/**
 * @file
 * @brief
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_VIRTIO_VIRTIO_IO_H_
#define DRIVERS_VIRTIO_VIRTIO_IO_H_

#include <asm/io.h>
#include <stdint.h>

/**
 * VirtIO IO operations
 */
#define __VIRTIO_LOAD(b)                                   \
	static inline uint##b##_t virtio_load##b(uint32_t reg, \
			unsigned long base_addr) {                     \
		return in##b(base_addr + reg);                     \
	}
#define __VIRTIO_STORE(b)                               \
	static inline void virtio_store##b(uint##b##_t val, \
			uint32_t reg, unsigned long base_addr ) {   \
		out##b(val, base_addr + reg);                   \
	}
#define __VIRTIO_ORIN(b)                                       \
	static inline void virtio_orin##b(uint##b##_t val,         \
			uint32_t reg, unsigned long base_addr) {           \
		out##b(val | in##b(base_addr + reg), base_addr + reg); \
	}
#define __VIRTIO_ANDIN(b)                                      \
	static inline void virtio_andin##b(uint##b##_t val,        \
			uint32_t reg, unsigned long base_addr) {           \
		out##b(val & in##b(base_addr + reg), base_addr + reg); \
	}

__VIRTIO_LOAD(8)
__VIRTIO_LOAD(16)
__VIRTIO_LOAD(32)
__VIRTIO_STORE(8)
__VIRTIO_STORE(16)
__VIRTIO_STORE(32)
__VIRTIO_ORIN(8)
__VIRTIO_ANDIN(8)

#undef __VIRTIO_LOAD
#undef __VIRTIO_STORE
#undef __VIRTIO_ORIN
#undef __VIRTIO_ANDIN

#endif /* DRIVERS_VIRTIO_VIRTIO_IO_H_ */
