/**
 * @file
 * @brief
 *
 * @date 13.08.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_VIRTIO_VIRTIO_H_
#define DRIVERS_VIRTIO_VIRTIO_H_

#include <asm/io.h>
#include <drivers/virtio/virtio_io.h>
#include <stdint.h>
#include <util/binalign.h>

/**
 * VirtIO Device Registers
 */
#define VIRTIO_REG_DEVICE_F 0x00 /* Device features */
#define VIRTIO_REG_GUEST_F  0x04 /* Guest features */
#define VIRTIO_REG_QUEUE_A  0x08 /* Queue address */
#define VIRTIO_REG_QUEUE_SZ 0x0C /* Queue size */
#define VIRTIO_REG_QUEUE_SL 0x0E /* Queue select */
#define VIRTIO_REG_QUEUE_N  0x10 /* Queue notify */
#define VIRTIO_REG_DEVICE_S 0x12 /* Device status */
#define VIRTIO_REG_ISR_S    0x13 /* ISR status */

/**
 * VirtIO Device Status
 */
#define VIRTIO_CONFIG_S_ACKNOWLEDGE 0x01 /* Guest OS has found the device and
											recognized it as a valid virtio
											device */
#define VIRTIO_CONFIG_S_DRIVER      0x02 /* Guest OS knows how to drive the
											device */
#define VIRTIO_CONFIG_S_DRIVER_OK   0x04 /* Driver is set up and ready to drive
											the device */
#define VIRTIO_CONFIG_S_FAILED      0x80 /* Something went wront */

/**
 * VirtIO Ring Alignment
 */
#define VIRTIO_VRING_ALIGN 0x1000

/**
 * VirtIO Feature Operations
 */
static inline int virtio_has_feature(uint32_t feature,
		unsigned long base_addr) {
	return feature & virtio_load32(VIRTIO_REG_DEVICE_F,
			base_addr);
}

static inline void virtio_set_feature(uint32_t feature,
		unsigned long base_addr) {
	virtio_store32(feature, VIRTIO_REG_GUEST_F, base_addr);
}

/**
 * VirtIO Queue Operations
 */
static inline void virtio_select_queue(uint16_t q_id,
		unsigned long base_addr) {
	virtio_store16(q_id, VIRTIO_REG_QUEUE_SL, base_addr);
}

static inline void virtio_notify_queue(uint16_t q_id,
		unsigned long base_addr) {
	virtio_store16(q_id, VIRTIO_REG_QUEUE_N, base_addr);
}

static inline uint16_t virtio_get_queue_size(
		unsigned long base_addr) {
	return virtio_load16(VIRTIO_REG_QUEUE_SZ, base_addr);
}

static inline void virtio_set_queue_addr(void *q_addr,
		unsigned long base_addr) {
	assert(binalign_check_bound((uintptr_t)q_addr,
				VIRTIO_VRING_ALIGN));
	virtio_store32((uintptr_t)q_addr / VIRTIO_VRING_ALIGN,
			VIRTIO_REG_QUEUE_A, base_addr);
}

/**
 * VirtIO Status Operations
 */
static inline void virtio_reset(unsigned long base_addr) {
	virtio_store8(0, VIRTIO_REG_DEVICE_S, base_addr);
}

static inline void virtio_add_status(uint16_t status,
		unsigned long base_addr) {
	virtio_orin8(status, VIRTIO_REG_DEVICE_S, base_addr);
}

static inline void virtio_del_status(uint16_t status,
		unsigned long base_addr) {
	virtio_andin8(~status, VIRTIO_REG_DEVICE_S, base_addr);
}

/**
 * VirtIO Interrupt Status Operations
 */
static inline uint8_t virtio_get_isr_status(
		unsigned long base_addr) {
	return virtio_load8(VIRTIO_REG_ISR_S, base_addr);
}

#endif /* DRIVERS_VIRTIO_VIRTIO_H_ */
