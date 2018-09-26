/**
 * @file viv_compat.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.09.2018
 *
 * @note: All these functions (or very simmiliar ones) can be found
 * in gallium/etnaviv, so this header is needed just to compile
 * simple etnaviv commands without building MESA (which takes pretty
 * long time)
 */

#ifndef VIV_COMPAT_H_
#define VIV_COMPAT_H_

#include "viv.h"

#include <xf86drm.h>
#include <etnaviv/etnaviv_drmif.h>
#include <etnaviv/etnaviv_drm.h>

#include <etnaviv_xml/common.xml.h>
#include <etnaviv_xml/state.xml.h>
#include <etnaviv_xml/state_2d.xml.h>
#include <etnaviv_xml/state_3d.xml.h>
#include <etnaviv_xml/cmdstream.xml.h>

extern int viv_open(enum viv_hw_type type, struct viv_conn **conn);

#define etna_bo_gpu_address(bo) (((uint32_t) etna_bo_map(bo) - 0x10000000))

static inline uint32_t etna_f32_to_u32(float value) {
	union {
		uint32_t u32;
		float f32;
	} x = { .f32 = value };
	return x.u32;
}

typedef viv_addr_t uint32_t;

static inline  void etna_emit_load_state(struct etna_cmd_stream *stream,
		const uint16_t offset, const uint16_t count) {
	uint32_t v;

	v = 	(VIV_FE_LOAD_STATE_HEADER_OP_LOAD_STATE |
			VIV_FE_LOAD_STATE_HEADER_OFFSET(offset) |
			(VIV_FE_LOAD_STATE_HEADER_COUNT(count) &
			 VIV_FE_LOAD_STATE_HEADER_COUNT__MASK));

	etna_cmd_stream_emit(stream, v);
}

static inline void
etna_set_state(struct etna_cmd_stream *stream, uint32_t address,
		uint32_t value) {
	etna_cmd_stream_reserve(stream, 2);
	etna_emit_load_state(stream, address >> 2, 1);
	etna_cmd_stream_emit(stream, value);
}

static inline void etna_set_state_from_bo(struct etna_cmd_stream *stream,
		uint32_t address, struct etna_bo *bo) {
	etna_cmd_stream_reserve(stream, 2);
	etna_emit_load_state(stream, address >> 2, 1);
	etna_cmd_stream_reloc(stream, &(struct etna_reloc){
			.bo = bo,
			.flags = ETNA_RELOC_READ,
			.offset = 0,
			});
}

static inline void etna_warm_up_rs(struct etna_cmd_stream *cmdPtr,
		uint32_t aux_rt_physical, uint32_t aux_rt_ts_physical) {
	/* ADDR_G */
	etna_set_state(cmdPtr, VIVS_TS_COLOR_STATUS_BASE, aux_rt_ts_physical);
	/* ADDR_F */
	etna_set_state(cmdPtr, VIVS_TS_COLOR_SURFACE_BASE, aux_rt_physical);
	etna_set_state(cmdPtr, VIVS_TS_FLUSH_CACHE, VIVS_TS_FLUSH_CACHE_FLUSH);
	etna_set_state(cmdPtr, VIVS_RS_CONFIG,  /* wut? */
			VIVS_RS_CONFIG_SOURCE_FORMAT(RS_FORMAT_A8R8G8B8) |
			VIVS_RS_CONFIG_SOURCE_TILED |
			VIVS_RS_CONFIG_DEST_FORMAT(RS_FORMAT_R5G6B5) |
			VIVS_RS_CONFIG_DEST_TILED);
	/* ADDR_F */
	etna_set_state(cmdPtr, VIVS_RS_SOURCE_ADDR, aux_rt_physical);
	etna_set_state(cmdPtr, VIVS_RS_SOURCE_STRIDE, 0x400 * 2);
	/* ADDR_F */
	etna_set_state(cmdPtr, VIVS_RS_PIPE_SOURCE_ADDR(0), aux_rt_physical);
	/* ADDR_F */
	etna_set_state(cmdPtr, VIVS_RS_DEST_ADDR, aux_rt_physical);
	etna_set_state(cmdPtr, VIVS_RS_DEST_STRIDE, 0x400 * 2);
	/* ADDR_F */
	etna_set_state(cmdPtr, VIVS_RS_PIPE_DEST_ADDR(0), aux_rt_physical);
	etna_set_state(cmdPtr, VIVS_RS_WINDOW_SIZE,
			VIVS_RS_WINDOW_SIZE_HEIGHT(4) |
			VIVS_RS_WINDOW_SIZE_WIDTH(16));
	etna_set_state(cmdPtr, VIVS_RS_CLEAR_CONTROL,
			VIVS_RS_CLEAR_CONTROL_MODE_DISABLED);
	etna_set_state(cmdPtr, VIVS_RS_KICKER, 0xbeebbeeb);
}

static inline void etna_set_state_f32(struct etna_cmd_stream *commandBuffer,
		uint32_t address, float value) {
	union {
		uint32_t i32;
		float f32;
	} x = { .f32 = value };
	etna_set_state(commandBuffer, address, x.i32);
}

/* Queue a STALL command (queues 2 words) */
static inline void
CMD_STALL(struct etna_cmd_stream *stream, uint32_t from, uint32_t to) {
	etna_cmd_stream_emit(stream, VIV_FE_STALL_HEADER_OP_STALL);
	etna_cmd_stream_emit(stream,
			VIV_FE_STALL_TOKEN_FROM(from) |
			VIV_FE_STALL_TOKEN_TO(to));
}

static inline void
etna_stall(struct etna_cmd_stream *stream, uint32_t from, uint32_t to) {
	etna_cmd_stream_reserve(stream, 4);

	etna_emit_load_state(stream, VIVS_GL_SEMAPHORE_TOKEN >> 2, 1);
	etna_cmd_stream_emit(stream,
			VIVS_GL_SEMAPHORE_TOKEN_FROM(from) |
			VIVS_GL_SEMAPHORE_TOKEN_TO(to));

	if (from == SYNC_RECIPIENT_FE) {
		/* if the frontend is to be stalled,
		 * queue a STALL frontend command */
		CMD_STALL(stream, from, to);
	} else {
		/* otherwise, load the STALL token state */
		etna_emit_load_state(stream, VIVS_GL_STALL_TOKEN >> 2, 1);
		etna_cmd_stream_emit(stream,
				VIVS_GL_STALL_TOKEN_FROM(from) |
				VIVS_GL_STALL_TOKEN_TO(to));
	}
}

static inline void etna_set_state_multi(struct etna_cmd_stream *commandBuffer,
		uint32_t base, uint32_t num, uint32_t *values) {
	uint32_t *tgt =
		(uint32_t *) (commandBuffer->buffer + commandBuffer->offset);
	tgt[0] = VIV_FE_LOAD_STATE_HEADER_OP_LOAD_STATE |
		VIV_FE_LOAD_STATE_HEADER_COUNT(num & 0x3ff) |
		VIV_FE_LOAD_STATE_HEADER_OFFSET(base >> 2);
	memcpy(&tgt[1], values, 4*num);
	commandBuffer->offset += (4 + num*4) / 4;
	if(commandBuffer->offset & (4 / 4)) /* PAD */ {
		commandBuffer->offset += 4 / 4;
	}
}

#undef etna_reserve
#undef ETNA_EMIT_LOAD_STATE
#undef ETNA_EMIT

#define etna_reserve		etna_cmd_stream_reserve
#define ETNA_EMIT_LOAD_STATE    etna_emit_load_state
#define ETNA_EMIT		etna_cmd_stream_emit

#define etna_finish etna_cmd_stream_finish

#define etna_flush(a, b)	 \
	{ etna_cmd_stream_flush(a); (a)->offset = (a)->size = 0 ;}

static inline void etna_set_state_fixp(struct etna_cmd_stream *commandBuffer,
		uint32_t address, uint32_t value) {
	uint32_t *tgt =
		(uint32_t *) (commandBuffer->buffer + commandBuffer->offset);
	tgt[0] = VIV_FE_LOAD_STATE_HEADER_OP_LOAD_STATE |
		VIV_FE_LOAD_STATE_HEADER_COUNT(1) |
		VIV_FE_LOAD_STATE_HEADER_FIXP |
		VIV_FE_LOAD_STATE_HEADER_OFFSET(address >> 2);
	tgt[1] = value;
	commandBuffer->offset += 8 / 4;
}

static inline void etna_draw_primitives(struct etna_cmd_stream *cmdPtr,
		uint32_t primitive_type, uint32_t start, uint32_t count) {
	uint32_t *tgt = (uint32_t*)(cmdPtr->buffer + cmdPtr->offset);
	tgt[0] = VIV_FE_DRAW_PRIMITIVES_HEADER_OP_DRAW_PRIMITIVES;
	tgt[1] = primitive_type;
	tgt[2] = start;
	tgt[3] = count;
	cmdPtr->offset += 16 / 4;
}

static inline uint32_t etna_align_up(uint32_t value, uint32_t granularity) {
    return (value + (granularity-1)) & (~(granularity-1));
}

static inline uint32_t etna_align_down(uint32_t value, uint32_t granularity) {
    return (value) & (~(granularity-1));
}

#endif /* VIV_COMPAT_H_ */
