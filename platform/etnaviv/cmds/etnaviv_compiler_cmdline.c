/*
 * Copyright (c) 2015 Etnaviv Project
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
 *
 * Authors:
 *    Rob Clark <robclark@freedesktop.org>
 *    Christian Gmeiner <christian.gmeiner@gmail.com>
 */

#include <err.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "tgsi/tgsi_dump.h"
#include "tgsi/tgsi_parse.h"
#include "tgsi/tgsi_text.h"

#include "etnaviv_compiler.h"
#include "etnaviv_debug.h"
#include "etnaviv_internal.h"
#include "etnaviv_shader.h"

#include "util/u_memory.h"

static const struct etna_specs specs_gc2000 = {
   .vs_need_z_div = 0,
   .has_sin_cos_sqrt = 1,
   .has_sign_floor_ceil = 1,
   .vertex_sampler_offset = 8,
   .vertex_output_buffer_size = 512,
   .vertex_cache_size = 16,
   .shader_core_count = 4,
   .max_instructions = 512,
   .max_varyings = 12,
   .max_registers = 64,
   .max_vs_uniforms = 168,
   .max_ps_uniforms = 128,
   .num_constants = 168,
};

static int
read_file(const char *file_name, void **ptr, size_t *rsize)
{
    char *text = NULL;
    size_t size;
    size_t total_read = 0;
    FILE *fp = fopen(file_name, "rb");

    if (!fp) {
        return 0;
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    text = (char *) malloc(size + 1);
    if (text != NULL) {
        do {
            size_t bytes = fread(text + total_read,
                         1, size - total_read, fp);
            if (bytes < size - total_read) {
                free(text);
                text = NULL;
                break;
            }

            if (bytes == 0) {
                break;
            }

            total_read += bytes;
        } while (total_read < size);

        text[total_read] = '\0';
    }

    fclose(fp);

    *ptr = text;

    *rsize = size;

    return 0;
}

static void
print_usage(void)
{
   printf("Usage: etnaviv_compiler [OPTIONS]... FILE\n");
   printf("    --verbose         - verbose compiler/debug messages\n");
   printf("    --frag-rb-swap    - swap rb in color output (FRAG)\n");
   printf("    --help            - show this message\n");
}

int
main(int argc, char **argv)
{
   int ret = 0, n = 1;
   const char *filename;
   struct tgsi_token toks[65536];
   struct tgsi_parse_context parse;
   struct etna_shader s = {};
   struct etna_shader_key key = {};
   void *ptr;
   size_t size;

   struct etna_shader_variant *v = CALLOC_STRUCT(etna_shader_variant);
   if (!v) {
      fprintf(stderr, "malloc failed!\n");
      return 1;
   }

   etna_mesa_debug = ETNA_DBG_MSGS;

   while (n < argc) {
      if (!strcmp(argv[n], "--verbose")) {
         etna_mesa_debug |= ETNA_DBG_COMPILER_MSGS;
         n++;
         continue;
      }

      if (!strcmp(argv[n], "--frag-rb-swap")) {
         debug_printf(" %s", argv[n]);
         key.frag_rb_swap = true;
         n++;
         continue;
      }

      if (!strcmp(argv[n], "--help")) {
         print_usage();
         return 0;
      }

      break;
   }

   filename = argv[n];

   if (n >= argc) {
	   printf("No fname given\n");
	   return 0;
   }

   ret = read_file(filename, &ptr, &size);
   if (ret) {
      print_usage();
      return ret;
   }

   debug_printf("%s\n", (char *)ptr);

   if (!tgsi_text_translate(ptr, toks, ARRAY_SIZE(toks)))
      errx(1, "could not parse `%s'", filename);

   tgsi_parse_init(&parse, toks);

   s.specs = &specs_gc2000;
   s.tokens = toks;

   v->shader = &s;
   v->key = key;

   if (!etna_compile_shader(v)) {
      fprintf(stderr, "compiler failed!\n");
      return 1;
   }

   etna_dump_shader(v);
   etna_destroy_shader(v);
}
