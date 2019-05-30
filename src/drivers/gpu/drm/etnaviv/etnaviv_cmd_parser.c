/*
 * Copyright (C) 2015 Etnaviv Project
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

#include <util/log.h>

#include "etnaviv_compat.h"
#include "etnaviv_gem.h"
#include "etnaviv_gpu.h"

#include <etnaviv_xml/cmdstream.xml.h>

#define EXTRACT(val, field) (((val) & field##__MASK) >> field##__SHIFT)

struct etna_validation_state {
	struct etnaviv_gpu *gpu;
	const struct drm_etnaviv_gem_submit_reloc *relocs;
	unsigned int num_relocs;
	uint32_t *start;
};

#define ETNAVIV_STATES_SIZE (VIV_FE_LOAD_STATE_HEADER_OFFSET__MASK + 1u)

static uint8_t cmd_length[32] = {
	[FE_OPCODE_DRAW_PRIMITIVES] = 4,
	[FE_OPCODE_DRAW_INDEXED_PRIMITIVES] = 6,
	[FE_OPCODE_DRAW_INSTANCED] = 4,
	[FE_OPCODE_NOP] = 2,
	[FE_OPCODE_STALL] = 2,
};

bool etnaviv_cmd_validate_one(struct etnaviv_gpu *gpu, uint32_t *stream,
			      unsigned int size,
			      struct drm_etnaviv_gem_submit_reloc *relocs,
			      unsigned int reloc_size)
{
	struct etna_validation_state state;
	uint32_t *buf = stream;
	uint32_t *end = buf + size;

	state.gpu = gpu;
	state.relocs = relocs;
	state.num_relocs = reloc_size;
	state.start = stream;

	log_debug("gpu(%p) stream(%p) size(%d) relocs(%p) reloc_size(%d)",  gpu,
			stream, size, relocs, reloc_size);
	while (buf < end) {
		uint32_t cmd = *buf;
		unsigned int len, n = 0, off;
		unsigned int op = cmd >> 27;
		log_debug("buf(%p) cmd(%x) op(%x)", buf, cmd, op);

		switch (op) {
		case FE_OPCODE_LOAD_STATE:
			n = EXTRACT(cmd, VIV_FE_LOAD_STATE_HEADER_COUNT);
			len = ALIGN(1 + n, 2);
			log_debug("FE_OPCODE_LOAD_STATE: n(%x) len(%x) end(%x)", n, len, end);
			if (buf + len > end)
				break;

			off = EXTRACT(cmd, VIV_FE_LOAD_STATE_HEADER_OFFSET);
			log_debug("FE_OPCODE_LOAD_STATE: n(%x) cmd(%x) off(%x)", n, len, off);
			break;

		case FE_OPCODE_DRAW_2D:
			n = EXTRACT(cmd, VIV_FE_DRAW_2D_HEADER_COUNT);
			if (n == 0)
				n = 256;
			len = 2 + n * 2;
			log_debug("FE_OPCODE_DRAW_2D: n(%x) len(%x)", n, len);
			break;

		default:
			len = cmd_length[op];
			log_debug("default: n(%x) len(%x) op(%x)", n, len, op);
			if (len == 0) {
				log_error("%s: op %u not permitted at offset %tu\n",
					__func__, op, buf - state.start);
				len = 0; /* Treat as NOP */
				buf = end;
			}
			break;
		}

		buf += len;
	}

	if (buf > end) {
		log_error("%s: commands overflow end of buffer: %tu > %u\n",
			__func__, buf - state.start, size);
		return false;
	}

	return true;
}
