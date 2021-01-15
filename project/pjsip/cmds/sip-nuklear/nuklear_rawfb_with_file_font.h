#ifndef NK_RAWFB_FILE_FONT_H_
#define NK_RAWFB_FILE_FONT_H_

/*
 * It's analog of nk_rawfb_init() from nuklear_rawfb.h,
 * but with support of external font.
 */

struct nk_font *rawfb_fonts[RAWFB_FONT_COUNT];

NK_API struct rawfb_context*
nk_rawfb_init_with_file_font(void *fb, void *tex_mem, const unsigned int w, const unsigned int h,
	    const unsigned int pitch, const rawfb_pl pl, const char *font_file,
	    float font_size[]) {
	const void *tex;
	struct rawfb_context *rawfb;
	struct nk_font_config cfg = nk_font_config(0);
	int i;

	rawfb = malloc(sizeof(struct rawfb_context));
	if (!rawfb) {
		return NULL;
	}

	if (font_size[0] == 0) {
		perror("No fonts passed\n");
		return NULL;
	}

	NK_MEMSET(rawfb, 0, sizeof(struct rawfb_context));
	rawfb->font_tex.pixels = tex_mem;
	rawfb->font_tex.format = NK_FONT_ATLAS_ALPHA8;
	rawfb->font_tex.w = rawfb->font_tex.h = 0;

	rawfb->fb.pixels = fb;
	rawfb->fb.w= w;
	rawfb->fb.h = h;
	rawfb->fb.pl = pl;

	if (pl == PIXEL_LAYOUT_RGBX_8888 || pl == PIXEL_LAYOUT_XRGB_8888) {
		rawfb->fb.format = NK_FONT_ATLAS_RGBA32;
		rawfb->fb.pitch = pitch;
	} else {
		perror("nk_rawfb_init(): Unsupported pixel layout.\n");
		free(rawfb);
		return NULL;
	}

	nk_font_atlas_init_default(&rawfb->atlas);
	nk_font_atlas_begin(&rawfb->atlas);

	for (i = 0; i < RAWFB_FONT_COUNT; i++) {
		if (!font_size[i]) {
			break;
		}
		rawfb_fonts[i] = nk_font_atlas_add_from_file(&rawfb->atlas, font_file, font_size[i], &cfg);
	}

	tex = nk_font_atlas_bake(&rawfb->atlas, &rawfb->font_tex.w, &rawfb->font_tex.h, rawfb->font_tex.format);
	if (!tex) {
		free(rawfb);
		return NULL;
	}

	if (0 == nk_init_default(&rawfb->ctx, &rawfb_fonts[0]->handle)) {
		free(rawfb);
		return NULL;
	}

	switch(rawfb->font_tex.format) {
	case NK_FONT_ATLAS_ALPHA8:
		rawfb->font_tex.pitch = rawfb->font_tex.w * 1;
		break;
	case NK_FONT_ATLAS_RGBA32:
		rawfb->font_tex.pitch = rawfb->font_tex.w * 4;
		break;
	};
	/* Store the font texture in tex scratch memory */
	memcpy(rawfb->font_tex.pixels, tex, rawfb->font_tex.pitch * rawfb->font_tex.h);
	nk_font_atlas_end(&rawfb->atlas, nk_handle_ptr(NULL), NULL);
	if (rawfb->atlas.default_font)
		nk_style_set_font(&rawfb->ctx, &rawfb->atlas.default_font->handle);
	nk_style_load_all_cursors(&rawfb->ctx, rawfb->atlas.cursors);
	nk_rawfb_scissor(rawfb, 0, 0, rawfb->fb.w, rawfb->fb.h);

    return rawfb;
}

#endif /* NK_RAWFB_FILE_FONT_H_ */
