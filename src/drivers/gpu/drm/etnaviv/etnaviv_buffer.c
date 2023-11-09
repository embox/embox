/*
 * Copyright (C) 2014 Etnaviv Project
 * Author: Christian Gmeiner <christian.gmeiner@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <inttypes.h>

#include <../arch/arm/armlib/mem_barriers.h>

#include "etnaviv_compat.h"
#include "etnaviv_cmdbuf.h"
#include "etnaviv_gpu.h"
#include "etnaviv_gem.h"
#include "etnaviv_drm.h"

#include <etnaviv_xml/common.xml.h>
#include <etnaviv_xml/state.xml.h>
#include <etnaviv_xml/state_hi.xml.h>
#include <etnaviv_xml/state_3d.xml.h>
#include <etnaviv_xml/cmdstream.xml.h>

/*
 * Command Buffer helper:
 */

static inline void OUT(struct etnaviv_cmdbuf *buffer, uint32_t data) {
	uint32_t *vaddr = (uint32_t *)buffer->vaddr;
	vaddr[buffer->user_size / 4] = data;
	buffer->user_size += 4;
}

static inline void CMD_LOAD_STATE(struct etnaviv_cmdbuf *buffer,
	uint32_t reg, uint32_t value) {
	uint32_t index = reg >> VIV_FE_LOAD_STATE_HEADER_OFFSET__SHR;

	buffer->user_size = ALIGN(buffer->user_size, 8);

	/* write a register via cmd stream */
	OUT(buffer, VIV_FE_LOAD_STATE_HEADER_OP_LOAD_STATE |
		    VIV_FE_LOAD_STATE_HEADER_COUNT(1) |
		    VIV_FE_LOAD_STATE_HEADER_OFFSET(index));
	OUT(buffer, value);
}

static inline void CMD_END(struct etnaviv_cmdbuf *buffer) {
	buffer->user_size = ALIGN(buffer->user_size, 8);

	OUT(buffer, VIV_FE_END_HEADER_OP_END);
}

static inline void CMD_WAIT(struct etnaviv_cmdbuf *buffer) {
	buffer->user_size = ALIGN(buffer->user_size, 8);

	OUT(buffer, VIV_FE_WAIT_HEADER_OP_WAIT | 200);
}

static inline void CMD_LINK(struct etnaviv_cmdbuf *buffer,
	uint16_t prefetch, uint32_t address) {
	buffer->user_size = ALIGN(buffer->user_size, 8);

	OUT(buffer, VIV_FE_LINK_HEADER_OP_LINK |
		    VIV_FE_LINK_HEADER_PREFETCH(prefetch));
	OUT(buffer, address);
}

static inline void CMD_STALL(struct etnaviv_cmdbuf *buffer,
	uint32_t from, uint32_t to) {
	buffer->user_size = ALIGN(buffer->user_size, 8);

	OUT(buffer, VIV_FE_STALL_HEADER_OP_STALL);
	OUT(buffer, VIV_FE_STALL_TOKEN_FROM(from) | VIV_FE_STALL_TOKEN_TO(to));
}

static inline void CMD_SEM(struct etnaviv_cmdbuf *buffer, uint32_t from, uint32_t to) {
	CMD_LOAD_STATE(buffer, VIVS_GL_SEMAPHORE_TOKEN,
		       VIVS_GL_SEMAPHORE_TOKEN_FROM(from) |
		       VIVS_GL_SEMAPHORE_TOKEN_TO(to));
}

static void etnaviv_cmd_select_pipe(struct etnaviv_gpu *gpu,
	struct etnaviv_cmdbuf *buffer, uint8_t pipe) {
	uint32_t flush = 0;

	/*
	 * This assumes that if we're switching to 2D, we're switching
	 * away from 3D, and vice versa.  Hence, if we're switching to
	 * the 2D core, we need to flush the 3D depth and color caches,
	 * otherwise we need to flush the 2D pixel engine cache.
	 */
	if (gpu->exec_state == ETNA_PIPE_2D)
		flush = VIVS_GL_FLUSH_CACHE_PE2D;
	else if (gpu->exec_state == ETNA_PIPE_3D)
		flush = VIVS_GL_FLUSH_CACHE_DEPTH | VIVS_GL_FLUSH_CACHE_COLOR;

	CMD_LOAD_STATE(buffer, VIVS_GL_FLUSH_CACHE, flush);
	CMD_SEM(buffer, SYNC_RECIPIENT_FE, SYNC_RECIPIENT_PE);
	CMD_STALL(buffer, SYNC_RECIPIENT_FE, SYNC_RECIPIENT_PE);

	CMD_LOAD_STATE(buffer, VIVS_GL_PIPE_SELECT,
		       VIVS_GL_PIPE_SELECT_PIPE(pipe));
}

void etnaviv_buffer_dump(struct etnaviv_gpu *gpu,
	struct etnaviv_cmdbuf *buf, uint32_t off, uint32_t len) {
	uint32_t size = buf->size;
	uint32_t *ptr = buf->vaddr + off;
	int i;

	log_debug("virt %p phys 0x%08"PRIu32" free 0x%08"PRIu32"\n", ptr,
			etnaviv_cmdbuf_get_va(buf) + off, size - len * 4 - off);

	for (i = 0; i < len / 4; i++) {
		if (i && !(i % 8))
			printk("\n");
		if (i % 8 == 0)
			printk("\t%p: ", ptr + i);
		printk("%08"PRIu32" ", *(ptr + i));
	}

	printk("\n");
}

/*
 * Safely replace the WAIT of a waitlink with a new command and argument.
 * The GPU may be executing this WAIT while we're modifying it, so we have
 * to write it in a specific order to avoid the GPU branching to somewhere
 * else.  'wl_offset' is the offset to the first byte of the WAIT command.
 */
static void etnaviv_buffer_replace_wait(struct etnaviv_cmdbuf *buffer,
	unsigned int wl_offset, uint32_t cmd, uint32_t arg) {
	uint32_t *lw = buffer->vaddr + wl_offset;

	lw[1] = arg;
	data_mem_barrier();
	lw[0] = cmd;
	data_mem_barrier();
}
/*
 * Ensure that there is space in the command buffer to contiguously write
 * 'cmd_dwords' 64-bit words into the buffer, wrapping if necessary.
 */
static uint32_t etnaviv_buffer_reserve(struct etnaviv_gpu *gpu,
	struct etnaviv_cmdbuf *buffer, unsigned int cmd_dwords) {
	if (buffer->user_size + cmd_dwords * sizeof(uint64_t) > buffer->size)
		buffer->user_size = 0;

	return etnaviv_cmdbuf_get_va(buffer) + buffer->user_size;
}

uint16_t etnaviv_buffer_init(struct etnaviv_gpu *gpu) {
	struct etnaviv_cmdbuf *buffer = gpu->buffer;

	/* initialize buffer */
	buffer->user_size = 0;

	CMD_WAIT(buffer);
	CMD_LINK(buffer, 2, etnaviv_cmdbuf_get_va(buffer) +
		 buffer->user_size - 4);

	return buffer->user_size / 8;
}

uint16_t etnaviv_buffer_config_mmuv2(struct etnaviv_gpu *gpu, uint32_t mtlb_addr,
	uint32_t safe_addr) {
	struct etnaviv_cmdbuf *buffer = gpu->buffer;

	buffer->user_size = 0;

	if (gpu->identity.features & chipFeatures_PIPE_3D) {
		CMD_LOAD_STATE(buffer, VIVS_GL_PIPE_SELECT,
			       VIVS_GL_PIPE_SELECT_PIPE(ETNA_PIPE_3D));
		CMD_LOAD_STATE(buffer, VIVS_MMUv2_CONFIGURATION,
			mtlb_addr | VIVS_MMUv2_CONFIGURATION_MODE_MODE4_K);
		CMD_LOAD_STATE(buffer, VIVS_MMUv2_SAFE_ADDRESS, safe_addr);
		CMD_SEM(buffer, SYNC_RECIPIENT_FE, SYNC_RECIPIENT_PE);
		CMD_STALL(buffer, SYNC_RECIPIENT_FE, SYNC_RECIPIENT_PE);
	}

	if (gpu->identity.features & chipFeatures_PIPE_2D) {
		CMD_LOAD_STATE(buffer, VIVS_GL_PIPE_SELECT,
			       VIVS_GL_PIPE_SELECT_PIPE(ETNA_PIPE_2D));
		CMD_LOAD_STATE(buffer, VIVS_MMUv2_CONFIGURATION,
			mtlb_addr | VIVS_MMUv2_CONFIGURATION_MODE_MODE4_K);
		CMD_LOAD_STATE(buffer, VIVS_MMUv2_SAFE_ADDRESS, safe_addr);
		CMD_SEM(buffer, SYNC_RECIPIENT_FE, SYNC_RECIPIENT_PE);
		CMD_STALL(buffer, SYNC_RECIPIENT_FE, SYNC_RECIPIENT_PE);
	}

	CMD_END(buffer);

	buffer->user_size = ALIGN(buffer->user_size, 8);

	return buffer->user_size / 8;
}

/* Append a command buffer to the ring buffer. */
void etnaviv_buffer_queue(struct etnaviv_gpu *gpu, unsigned int event,
	struct etnaviv_cmdbuf *cmdbuf) {
	struct etnaviv_cmdbuf *buffer = gpu->buffer;
	unsigned int waitlink_offset = buffer->user_size - 16;
	uint32_t return_target, return_dwords;
	uint32_t link_target, link_dwords;

	log_debug("exec_state=%d", gpu->exec_state);
	link_target = etnaviv_cmdbuf_get_va(cmdbuf);
	link_dwords = cmdbuf->size / 8;
	/*
	 * If we need maintanence prior to submitting this buffer, we will
	 * need to append a mmu flush load state, followed by a new
	 * link to this buffer - a total of four additional words.
	 */
	if (gpu->mmu.need_flush || gpu->switch_context) {
		uint32_t target, extra_dwords;

		/* link command */
		extra_dwords = 1;

		/* flush command */
		if (gpu->mmu.need_flush) {
			if (gpu->mmu.version == ETNAVIV_IOMMU_V1)
				extra_dwords += 1;
			else
				extra_dwords += 3;
		}

		/* pipe switch commands */
		if (gpu->switch_context) {
			extra_dwords += 4;
		}

		target = etnaviv_buffer_reserve(gpu, buffer, extra_dwords);

		if (gpu->mmu.need_flush) {
			/* Add the MMU flush */
			if (gpu->mmu.version == ETNAVIV_IOMMU_V1) {
				CMD_LOAD_STATE(buffer, VIVS_GL_FLUSH_MMU,
					       VIVS_GL_FLUSH_MMU_FLUSH_FEMMU |
					       VIVS_GL_FLUSH_MMU_FLUSH_UNK1 |
					       VIVS_GL_FLUSH_MMU_FLUSH_UNK2 |
					       VIVS_GL_FLUSH_MMU_FLUSH_PEMMU |
					       VIVS_GL_FLUSH_MMU_FLUSH_UNK4);
			} else {
				CMD_LOAD_STATE(buffer, VIVS_MMUv2_CONFIGURATION,
					VIVS_MMUv2_CONFIGURATION_MODE_MASK |
					VIVS_MMUv2_CONFIGURATION_ADDRESS_MASK |
					VIVS_MMUv2_CONFIGURATION_FLUSH_FLUSH);
				CMD_SEM(buffer, SYNC_RECIPIENT_FE,
					SYNC_RECIPIENT_PE);
				CMD_STALL(buffer, SYNC_RECIPIENT_FE,
					SYNC_RECIPIENT_PE);
			}

			gpu->mmu.need_flush = false;
		}

		if (gpu->switch_context) {
			etnaviv_cmd_select_pipe(gpu, buffer, cmdbuf->exec_state);
			gpu->exec_state = cmdbuf->exec_state;
			gpu->switch_context = false;
		}

		/* And the link to the submitted buffer */
		CMD_LINK(buffer, link_dwords, link_target);

		/* Update the link target to point to above instructions */
		link_target = target;
		link_dwords = extra_dwords;
	}

	/*
	 * Append a LINK to the submitted command buffer to return to
	 * the ring buffer.  return_target is the ring target address.
	 * We need at most 7 dwords in the return target: 2 cache flush +
	 * 2 semaphore stall + 1 event + 1 wait + 1 link.
	 */
	return_dwords = 7;
	return_target = etnaviv_buffer_reserve(gpu, buffer, return_dwords);
	CMD_LINK(cmdbuf, return_dwords, return_target);

	/*
	 * Append a cache flush, stall, event, wait and link pointing back to
	 * the wait command to the ring buffer.
	 */
	if (gpu->exec_state == ETNA_PIPE_2D) {
		CMD_LOAD_STATE(buffer, VIVS_GL_FLUSH_CACHE,
				       VIVS_GL_FLUSH_CACHE_PE2D);
	 } else  {
		CMD_LOAD_STATE(buffer, VIVS_GL_FLUSH_CACHE,
				       VIVS_GL_FLUSH_CACHE_DEPTH |
				       VIVS_GL_FLUSH_CACHE_COLOR);
		CMD_LOAD_STATE(buffer, VIVS_TS_FLUSH_CACHE,
				       VIVS_TS_FLUSH_CACHE_FLUSH);
	}

	CMD_SEM(buffer, SYNC_RECIPIENT_FE, SYNC_RECIPIENT_PE);
	CMD_STALL(buffer, SYNC_RECIPIENT_FE, SYNC_RECIPIENT_PE);
	CMD_LOAD_STATE(buffer, VIVS_GL_EVENT, VIVS_GL_EVENT_EVENT_ID(event) |
		       VIVS_GL_EVENT_FROM_PE);
	CMD_WAIT(buffer);
	CMD_LINK(buffer, 2, etnaviv_cmdbuf_get_va(buffer) +
			    buffer->user_size - 4);

	etnaviv_buffer_dump(gpu, buffer, 0, buffer->user_size);
	log_debug("stream link to 0x%08x @ 0x%08x",
			return_target, etnaviv_cmdbuf_get_va(cmdbuf));
	log_debug("link op: %p", buffer->vaddr + waitlink_offset);
	log_debug("addr: 0x%08x", link_target);
	log_debug("back: 0x%08x", return_target);
	log_debug("event: %d", event);

	/*
	 * Kick off the submitted command by replacing the previous
	 * WAIT with a link to the address in the ring buffer.
	 */
	etnaviv_buffer_replace_wait(buffer, waitlink_offset,
				    VIV_FE_LINK_HEADER_OP_LINK |
				    VIV_FE_LINK_HEADER_PREFETCH(link_dwords),
				    link_target);

	etnaviv_buffer_dump(gpu, buffer, 0, buffer->user_size);
}
