/*
 * Copyright (c) 2012-2013 Etnaviv Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/* Thin wrapper around Vivante ioctls */
#ifndef H_VIV
#define H_VIV

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

/* Enum with indices for each of the feature words */
enum viv_features_word
{
    viv_chipFeatures = 0,
    viv_chipMinorFeatures0 = 1,
    viv_chipMinorFeatures1 = 2,
    viv_chipMinorFeatures2 = 3,
    viv_chipMinorFeatures3 = 4,
    VIV_FEATURES_WORD_COUNT /* Must be last */
};

/* hardware type */
/* matches gceHARDWARE_TYPE enums */
enum viv_hw_type
{
    VIV_HW_3D = 1,
    VIV_HW_2D = 2,
    VIV_HW_VG = 4,

    VIV_HW_2D3D = VIV_HW_3D | VIV_HW_2D
};

/* Type for GPU physical address */
typedef uint32_t viv_addr_t;

/* General process handle */
typedef uint64_t viv_handle_t;

/* Memory node handle */
typedef uint64_t viv_node_t;

/* GPU context handle */
typedef uint64_t viv_context_t;

/* User memory info handle */
typedef uint64_t viv_usermem_t;

/* kernel-interface independent chip specs structure, this is much easier to use
 * than checking GCABI defines all the time.
 */
struct viv_specs {
    uint32_t chip_model;
    uint32_t chip_revision;
    uint32_t chip_features[VIV_FEATURES_WORD_COUNT];
    uint32_t stream_count;
    uint32_t register_max;
    uint32_t thread_count;
    uint32_t shader_core_count;
    uint32_t vertex_cache_size;
    uint32_t vertex_output_buffer_size;
    uint32_t pixel_pipes;
    uint32_t instruction_count;
    uint32_t num_constants;
    uint32_t buffer_size;
    uint32_t varyings_count;
};

/* Structure encompassing a connection to kernel driver */
struct viv_conn {
    int fd;
    enum viv_hw_type hw_type;
    struct viv_specs chip;
};

#endif
