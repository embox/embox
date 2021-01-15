#define WIN_WIDTH      800
#define WIN_HEIGHT     480
#define CALL_BTN_HEIGHT  ((WIN_WIDTH - 9 * 4) / 9)
#define CALL_INFO_TEXTBOX_HEIGHT 50

extern struct demo_call_info *call_info;

static struct nk_color demo_nk_btn_color_normal(void) {
	switch (call_info->state) {
	case CALL_INACTIVE:
	case CALL_INCOMING:
		return nk_rgb(0,150,0);
	case CALL_ACTIVE:
		return nk_rgb(150,0,0);
	default:
		return nk_rgb(0,0,0);
	}
}

static struct nk_color demo_nk_btn_color_hover(void) {
	switch (call_info->state) {
	case CALL_INACTIVE:
	case CALL_INCOMING:
		return nk_rgb(0,200,0);
	case CALL_ACTIVE:
		return nk_rgb(200,0,0);
	default:
		return nk_rgb(0,0,0);
	}
}

static struct nk_color demo_nk_btn_color_active(void) {
	switch (call_info->state) {
	case CALL_INACTIVE:
	case CALL_INCOMING:
		return nk_rgb(0,200,0);
	case CALL_ACTIVE:
		return nk_rgb(200,0,0);
	default:
		return nk_rgb(0,0,0);
	}
}

static void demo_nk_accept_btn(struct nk_context *ctx,
	struct nk_image *img, int *pressed) {

	nk_style_push_style_item(ctx, &ctx->style.button.normal,
			nk_style_item_color(demo_nk_btn_color_normal()));
	nk_style_push_style_item(ctx, &ctx->style.button.hover,
			nk_style_item_color(demo_nk_btn_color_hover()));
	nk_style_push_style_item(ctx, &ctx->style.button.active,
			nk_style_item_color(demo_nk_btn_color_active()));

    if (nk_button_image(ctx, *img)) {
        fprintf(stdout, "answer pressed\n");
		*pressed = 1;
	}

	nk_style_pop_style_item(ctx);
	nk_style_pop_style_item(ctx);
	nk_style_pop_style_item(ctx);
}

static void demo_nk_decline_btn(struct nk_context *ctx,
	struct nk_image *img, int *pressed) {

	nk_style_push_style_item(ctx, &ctx->style.button.normal,
			nk_style_item_color(nk_rgb(150, 0, 0)));
	nk_style_push_style_item(ctx, &ctx->style.button.hover,
			nk_style_item_color(nk_rgb(200, 0, 0)));
	nk_style_push_style_item(ctx, &ctx->style.button.active,
			nk_style_item_color(nk_rgb(200, 0, 0)));

    if (nk_button_image(ctx, *img)) {
    	fprintf(stdout, "decline pressed\n");
		*pressed = 1;
	}

	nk_style_pop_style_item(ctx);
	nk_style_pop_style_item(ctx);
	nk_style_pop_style_item(ctx);
}

static void demo_nk_gui(struct nk_context *ctx, struct nk_image *im_accept,
		struct nk_image *im_decline, int draw_mouse, int *redraw_required) {
	*redraw_required = 0;

	if (nk_begin(ctx, "Demo", nk_rect(0, 0, WIN_WIDTH, WIN_HEIGHT),
			NK_WINDOW_NO_SCROLLBAR)) {
		int answer_pressed = 0, decline_pressed = 0;

		if (!draw_mouse) {
			nk_style_hide_cursor(ctx);
		}

		nk_layout_row_static(ctx,
			(WIN_HEIGHT - CALL_BTN_HEIGHT - 2 * CALL_INFO_TEXTBOX_HEIGHT - WIN_HEIGHT / 4), 15, 1);

		nk_layout_row_dynamic(ctx, CALL_INFO_TEXTBOX_HEIGHT, 1);

		nk_style_set_font(ctx, &rawfb_fonts[RAWFB_FONT_DEFAULT]->handle);

		switch (call_info->state) {
		case CALL_INACTIVE:
			rawfb_fonts[RAWFB_FONT_DEFAULT]->handle.height = 56;
			nk_label(ctx, "PJSIP demo", NK_TEXT_CENTERED);
			rawfb_fonts[RAWFB_FONT_DEFAULT]->handle.height = 32;
			break;
		case CALL_INCOMING:
			rawfb_fonts[RAWFB_FONT_DEFAULT]->handle.height = 32;
			nk_label(ctx, "Incoming call from:", NK_TEXT_CENTERED);
			rawfb_fonts[RAWFB_FONT_DEFAULT]->handle.height = 38;
			nk_label(ctx, call_info->incoming, NK_TEXT_CENTERED);
			rawfb_fonts[RAWFB_FONT_DEFAULT]->handle.height = 32;
			break;
		case CALL_ACTIVE:
			rawfb_fonts[RAWFB_FONT_DEFAULT]->handle.height = 32;
			nk_label(ctx, "Active call:", NK_TEXT_CENTERED);
			rawfb_fonts[RAWFB_FONT_DEFAULT]->handle.height = 38;
			nk_label(ctx, call_info->remote_uri, NK_TEXT_CENTERED);
			rawfb_fonts[RAWFB_FONT_DEFAULT]->handle.height = 32;
			break;
		}

		if (call_info->state != CALL_INACTIVE) {
			nk_layout_row_static(ctx, (WIN_WIDTH - 9 * 4) / 9, (WIN_WIDTH - 9 * 4) / 9, 9);

			switch (call_info->state) {
			case CALL_INCOMING:
				nk_spacing(ctx, 2);
				demo_nk_accept_btn(ctx, im_accept, &answer_pressed);
				nk_spacing(ctx, 3);
				demo_nk_decline_btn(ctx, im_decline, &decline_pressed);
				nk_spacing(ctx, 2);
				break;
			case CALL_ACTIVE:
				nk_spacing(ctx, 4);
				demo_nk_decline_btn(ctx, im_decline, &decline_pressed);
				nk_spacing(ctx, 4);
				break;
			default:
				break;
			}
		}

		if (answer_pressed && call_info->state == CALL_INCOMING) {
			demo_pj_answer();
			*redraw_required = 1;
		}
		if (decline_pressed) {
			demo_pj_hang();
			*redraw_required = 1;
		}
	}
	nk_end(ctx);
}
