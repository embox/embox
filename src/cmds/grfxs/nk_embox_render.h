/* This file provides rendering primitives for embox discribed by nuklear.h */

/* includes from embox */
#include <drivers/console/mpx.h>
#include <drivers/video/font.h>

extern const struct font_desc font_vga_8x8, font_vga_8x16;

#define ABS(a) (((a) < 0) ? -(a) : (a)) 
#define MIN(a, b) ((a < b) ? a : b) 
#define MAX(a, b) ((a > b) ? a : b) 
#define SIGN(a)   ((a > 0) ? 1 : -1)

//extern unsigned char **images;


int rgba_to_device_color(struct vc *vc, uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha){
    switch (vc->fb->var.bits_per_pixel){
        case 16: {
            uint32_t b = (blue / 8)  & (0x1f);
            uint32_t g = ((green / 8)<<6) & (0x7e0);
            uint32_t r = ((red / 8)<<11) & 0xf800;
            return r | g | b;
        }
        case 24: {
            uint32_t b = blue;
            uint32_t g = green<<8;
            uint32_t r = red<<16;
            return  r | g | b;
        }
        default: return -1;
    }
}

void embox_fill_rect(struct vc *vc, int x, int y, int w, int h, int col){
    struct fb_fillrect rect;
    rect.dx = x;
    rect.dy = y;
    rect.width = w;
    rect.height = h;
    rect.color = col;
    rect.rop = ROP_COPY;
    fb_fillrect(vc->fb, &rect);

}
void embox_fill_triangle(struct vc *vc, int ax, int ay, int bx, int by, int cx, int cy, int col){
    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

    int if1, if2, if3;


    for (int i = MIN(MIN(ax, bx), cx); i <= MAX(MAX(ax, bx), cx); ++i){
        for (int j = MIN(MIN(ay, by), cy); j <= MAX(MAX(ay, by), cy); ++j){
            /* condition of belonging of the point to the triangle */
            if1 = (ax - i) * (by - ay) - (bx - ax) * (ay - j);
            if2 = (bx - i) * (cy - by) - (cx - bx) * (by - j);
            if3 = (cx - i) * (ay - cy) - (ax - cx) * (cy - j);

            if ( (if1 >= 0 && if2 >= 0 && if3 >= 0) || (if1 <= 0 && if2 <= 0 && if3 <= 0) ){
                rect.dx = i;
                rect.dy = j;
                fb_fillrect(vc->fb, &rect);
            }
        }
    }
}   
void embox_stroke_line(struct vc *vc, float ax, float ay, float bx, float by,int col, float thickness){
  
    float th = thickness / 2;

    if (ax == bx){
        embox_fill_rect(vc, ax - th, ay, thickness, ABS(ay-by), col);
        return;
    }
    if (ay == by){
        embox_fill_rect(vc, ax, ay - th, ABS(ax-bx), thickness, col);
        return;
    }
    if (ax > bx){
        int t = ax;
        ax = bx;
        bx = t;
        
        t = ay; 
        ay = by;
        by = t;
    }

    
    
    float tg = (bx - ax) / (by - ay);
    float dx = sqrt(th*th / (1 + tg*tg))
         ,dy = ABS(tg) * dx;
     
    float Ax = ax - dx
         ,Ay = ay + SIGN(tg)*dy
         ,Bx = ax + dx
         ,By = ay - SIGN(tg)*dy
         ,Cx = bx + dx
         ,Cy = by - SIGN(tg)*dy
         ,Dx = bx - dx
         ,Dy = by + SIGN(tg)*dy;

    embox_fill_triangle(vc, Ax, Ay, Bx, By, Cx, Cy, col);
    embox_fill_triangle(vc, Ax, Ay, Dx, Dy, Cx, Cy, col);

}
void embox_fill_circle(struct vc *vc, int cx, int cy, int r, int col){
    if (r == 0) return;

    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

    for (int i = cx - r; i <= cx + r; ++i){
        for (int j = cy - r; j <= cy + r; ++j){
            if ((i - cx)*(i - cx) + (j - cy) * (j - cy) <= r*r){
                rect.dx = i;
                rect.dy = j;
                fb_fillrect(vc->fb, &rect);
            }
        }
    }
}
void embox_fill_arc(struct vc *vc, float cx, float cy, float r, float a_min, float a_max, int col){
    if (r == 0) return;


    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;


    for (float a = a_min; a < a_max; a = a + 0.01){
        for (int i = 0; i < r; i++){
            rect.dx = cx + i * NK_COS(a);
            rect.dy = cy + i * NK_SIN(a);
            fb_fillrect(vc->fb, &rect);
        }
    }
}
void embox_stroke_arc(struct vc *vc, float cx, float cy, float r, float a_min, float a_max, int col, float thickness){
    if (r == 0) return;

    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;
    
    for (float a = a_min; a < a_max; a = a + 0.01){
        for (float th = 0; th <= thickness; th = th + 0.1){
            rect.dx = cx + (r - th) * NK_COS(a);
            rect.dy = cy + (r - th) * NK_SIN(a);
            fb_fillrect(vc->fb, &rect);
        }
    }
}
void embox_stroke_triangle(struct vc *vc, float ax, float ay, float bx, float by, float cx, float cy, int col, float thickness){
    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

    float if1, if2, if3, if4, if5, if6;
    float th = thickness;
    /* ox, oy - center then do scale*/
    float ox = (ax + bx + cx) / 3
         ,oy = (ay + by + cy) / 3
         ,ax2 = ox + (ax - ox) * (th - 1) / th
         ,ay2 = oy + (ay - oy) * (th - 1) / th
         ,bx2 = ox + (bx - ox) * (th - 1) / th
         ,by2 = oy + (by - oy) * (th - 1) / th
         ,cx2 = ox + (cx - ox) * (th - 1) / th
         ,cy2 = oy + (cy - oy) * (th - 1) / th;


    for (int i = MIN(MIN(ax, bx), cx); i <= MAX(MAX(ax, bx), cx); ++i){
        for (int j = MIN(MIN(ay, by), cy); j <= MAX(MAX(ay, by), cy); ++j){
            /* condition of belonging of the point to the triangle */
            if1 = (ax - i) * (by - ay) - (bx - ax) * (ay - j);
            if2 = (bx - i) * (cy - by) - (cx - bx) * (by - j);
            if3 = (cx - i) * (ay - cy) - (ax - cx) * (cy - j);

            
            if4 = (ax2 - i) * (by2 - ay2) - (bx2 - ax2) * (ay2 - j);
            if5 = (bx2 - i) * (cy2 - by2) - (cx2 - bx2) * (by2 - j);
            if6 = (cx2 - i) * (ay2 - cy2) - (ax2 - cx2) * (cy2 - j);


            if ( ( (if1 >= 0 && if2 >= 0 && if3 >= 0) || (if1 <= 0 && if2 <= 0 && if3 <= 0) )
            && !(  (if4  >= 0 && if5 >= 0 && if6 >= 0) || (if4 <= 0 && if5 <= 0 && if6 <= 0) ) ) {
                rect.dx = i;
                rect.dy = j;
                fb_fillrect(vc->fb, &rect);
            }
        }
    }
}   
void embox_stroke_rect(struct vc *vc, float x, float y, float w, float h, int col, float rounding, float thickness){
    int th = thickness / 2;
    struct fb_fillrect rect;
    rect.width = thickness;
    rect.height = thickness;
    rect.color = col;
    rect.rop = ROP_COPY;

    for (int i = x + th ; i <= x + w - th; i++){
            rect.dx = i;
            rect.dy = y + th;
            fb_fillrect(vc->fb, &rect);
            
            rect.dx = i;
            rect.dy = y + h - th;
            fb_fillrect(vc->fb, &rect);
        
    }
     for (int j = y + th; j <= y + h - th; j++){
            rect.dx = x + th;
            rect.dy = j;
            fb_fillrect(vc->fb, &rect);
            
            rect.dx = x + w - th;
            rect.dy = j;
            fb_fillrect(vc->fb, &rect);

    }

}
void embox_stroke_curve(struct vc *vc, int *x, int *y, int col, float thickness){

    float L0, L1, L2, L3;
    float j;
    float th = 1;//thickness /2;

    y[1] = (y[0] + y[1]) / 2;
    y[2] = (y[2] + y[3]) / 2;

    /* interpolation Lagrange */
    for (float i = x[0]; i <= x[3]; i = i + 0.1){
        L0 = (i - x[1]) * (i - x[2]) * (i - x[3]) / (x[0] - x[1]) / (x[0] - x[2]) / (x[0] - x[3]);
        L1 = (i - x[0]) * (i - x[2]) * (i - x[3]) / (x[1] - x[0]) / (x[1] - x[2]) / (x[1] - x[3]);
        L2 = (i - x[0]) * (i - x[1]) * (i - x[3]) / (x[2] - x[0]) / (x[2] - x[1]) / (x[2] - x[3]);
        L3 = (i - x[0]) * (i - x[1]) * (i - x[2]) / (x[3] - x[0]) / (x[3] - x[1]) / (x[3] - x[2]);

        j = y[0] * L0 + y[1] * L1 + y[2] * L2 + y[3] * L3;

        embox_fill_circle(vc, i, j, th, col);
    }
} 
void embox_stroke_circle(struct vc *vc, int cx, int cy, int r, int col, float thickness){
    struct fb_fillrect rect;
    rect.width = 1;
    rect.height = 1;
    rect.color = col;
    rect.rop = ROP_COPY;

  for (float a = 0; a < 2 * 3.14; a = a + 0.01){
        for (float th = 0; th <= thickness ; th = th + 0.1){
            rect.dx = cx + (r - th) * NK_COS(a) ;
            rect.dy = cy + (r - th) * NK_SIN(a);
            fb_fillrect(vc->fb, &rect);
        }
    }


}
void embox_add_text(struct vc *vc, int x, int y, int fg_color, int bg_color, const char *text, int len){
    struct fb_image symbol;
    symbol.dx = x;
    symbol.dy = y;
    symbol.width = 8; //t->w
    symbol.height = 8; //t->h
    symbol.fg_color = fg_color;
    symbol.bg_color = bg_color;
    symbol.depth = 1;

    char *cbuf = (char *) text;
    size_t nbyte = len;
    
    while (nbyte--) {
        symbol.data = (char *)(font_vga_8x8.data) + (unsigned char)(*cbuf++)*8; 
        fb_imageblit(vc->fb, &symbol);
        symbol.dx += 8;
    } 

}


int image_reg_x, image_reg_y, image_reg_w, image_reg_h, image_x, image_y, image_w, image_h;
void embox_add_image(struct vc *vc, struct nk_image img, int x, int y, int w, int h, int color){
    image_reg_x = img.region[0]; 
    image_reg_y = img.region[1];
    image_reg_w = img.region[2];
    image_reg_h = img.region[3];
    image_x = x;
    image_y = y;
    image_w = w;
    image_h = h;

    int or_w, or_h, or_format;

    //if (image_w * image_h == 0 )
    {
        stbi_load("gwen.png", &or_w, &or_h, &or_format, 0);
    }
}


// void embox_add_image(struct vc *vc, struct nk_image img, int x, int y, int w, int h, int color){
//     struct fb_fillrect rect;
//     rect.width = 1;
//     rect.height = 1;
//     rect.rop = ROP_COPY;rect.color = rgba_to_device_color(vc, nRed, nGreen, nBlue, nAlpha);
            
//             if (nAlpha < 3)
//                 continue;            
            
//             fb_fillrect(vc->fb, &rect);

    
//     // this is coordinates which will be translated from need coordinates to origin picture
//     int origX, origY;

//     for (int nY = 0; nY <= h; nY++)
//     {
// 	    for (float nX = 0; nX <= w; nX++)
//     {
//             rect.dx = x + nX;
//             rect.dy = y + nY;

//             origX = (nX * img.region[2]) / w;
//             origY = (nY * img.region[3]) / h;

//             /* offset in pixet array according to the image format*/
//             uint32_t nOffset = (origX + int n = orig[i]*4;

//            img.region[0] + (origY + img.region[1]) * img.w) * 4;

//             /* for skinning example */
//             // int nRed = (int)images[img.handle.id][nOffset+0];
//             // int nGreen = (int)images[img.handle.id][nOffset+1];
//             // int nBlue = (int)images[img.handle.id][nOffset+2];
//             // int nAlpha = (int)images[img.handle.id][nOffset+3];
            
//             int nRed = (int)images[0][nOffset+0];
//             int nGreen = (int)images[0][nOffset+1];
//             int nBlue = (int)images[0][nOffset+2];
//             int nAlpha = (int)images[0][nOffset+3];
            
            
//             rect.color = rgba_to_device_color(vc, nRed, nGreen, nBlue, nAlpha);
            
//             if (nAlpha < 3)
//                 continue;            
            
//             fb_fillrect(vc->fb, &rect);

//         }
//     } 
// }


static void draw( struct vc *vc, struct nk_context *ctx, int width, int height){ 
     /* shouldn't draw when window is off */
    if (!vc->fb) {
        nk_clear(ctx);
        return;
    }
    const struct nk_command *cmd;

    /* iterate over and execute each draw command */
    nk_foreach(cmd, ctx){
        switch (cmd->type) {
        case NK_COMMAND_NOP: break;
        case NK_COMMAND_LINE: {
            const struct nk_command_line *c = (const struct nk_command_line*)cmd;
            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);
            embox_stroke_line(vc, c->begin.x, c->begin.y, c->end.x, c->end.y, color, c->line_thickness);
        } break;
        case NK_COMMAND_CURVE: {
            const struct nk_command_curve *c = (const struct nk_command_curve*)cmd;
            int x[4];
            int y[4];

            x[0] = c->begin.x;
            x[1] = c->ctrl[0].x;
            x[2] = c->ctrl[1].x;
            x[3] = c->end.x;

            y[0] = c->begin.y;
            y[1] = c->ctrl[0].y;
            y[2] = c->ctrl[1].y;
            y[3] = c->end.y;

            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);     
            embox_stroke_curve(vc, x, y, color, c->line_thickness);
        } break;
        case NK_COMMAND_RECT: {
            const struct nk_command_rect *c = (const struct nk_command_rect*)cmd;
            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);
            embox_stroke_rect(vc, c->x, c->y, c->w, c->h, color, (float)c->rounding, c->line_thickness);
        } break; 
        case NK_COMMAND_RECT_FILLED: {
            const struct nk_command_rect_filled *c = (const struct nk_command_rect_filled*)cmd;
            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);
            if (!((c->x + c->w >= width) && (c->y + c->h >= height)))	
               embox_fill_rect(vc, c->x, c->y, c->w, c->h, color);
        }break;
        case NK_COMMAND_CIRCLE: {
            const struct nk_command_circle *c = (const struct nk_command_circle*)cmd;
            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);
            embox_stroke_circle(vc,(float)c->x + (float)c->w/2, (float)c->y + (float)c->h/2, (float)c->w/2, color, c->line_thickness);
        } break;
        case NK_COMMAND_CIRCLE_FILLED: {
            const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);
            embox_fill_circle(vc, c->x + c->w/2, c->y + c->h/2, c->w/2, color);
        } break;
        case NK_COMMAND_ARC: {
            const struct nk_command_arc *c = (const struct nk_command_arc*)cmd;
            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);            
            embox_stroke_arc(vc, c->cx, c->cy, c->r, c->a[0], c->a[1], color, c->line_thickness);
        } break;
        case NK_COMMAND_ARC_FILLED: {
            const struct nk_command_arc_filled *c = (const struct nk_command_arc_filled*)cmd;
            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);           
            embox_fill_arc(vc, c->cx, c->cy, c->r, c->a[0], c->a[1], color);
        } break;
        case NK_COMMAND_TRIANGLE: {
            const struct nk_command_triangle *c = (const struct nk_command_triangle*)cmd;
            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);           
            embox_stroke_triangle(vc,c->a.x, c->a.y, c->b.x, c->b.y, c->c.x, c->c.y, color, c->line_thickness);
        } break;
        case NK_COMMAND_TRIANGLE_FILLED: {
            const struct nk_command_triangle_filled *c = (const struct nk_command_triangle_filled*)cmd;
            int color = rgba_to_device_color(vc, c->color.r, c->color.g, c->color.b, c->color.a);           
            embox_fill_triangle(vc, c->a.x, c->a.y, c->b.x, c->b.y, c->c.x, c->c.y, color);
        } break;
        case NK_COMMAND_TEXT: {
            const struct nk_command_text *c = (const struct nk_command_text*)cmd;
            int fg_color = rgba_to_device_color(vc, c->foreground.r, c->foreground.g, c->foreground.b, c->foreground.a); 
            int bg_color = rgba_to_device_color(vc, c->background.r, c->background.g, c->background.b, c->background.a); 
            embox_add_text(vc, c->x, c->y, fg_color, bg_color, c->string, c->length );
        } break;
        case NK_COMMAND_IMAGE: {
            const struct nk_command_image *c = (const struct nk_command_image*)cmd;
            int color = rgba_to_device_color(vc, c->col.r, c->col.g, c->col.b, c->col.a);
            embox_add_image(vc, c->img, c->x, c->y, c->w, c->h, color);
        } break;

        /* unrealized primitives */
        /*
        case NK_COMMAND_SCISSOR: {
            const struct nk_command_scissor *s = (const struct nk_command_scissor*)cmd;
            nk_draw_list_add_clip(&ctx->draw_list, nk_rect(s->x, s->y, s->w, s->h));
        } break;
        case NK_COMMAND_POLYGON: {
            int i;
            const struct nk_command_polygon*p = (const struct nk_command_polygon*)cmd;
            for (i = 0; i < p->point_count; ++i) {
                struct nk_vec2 pnt = nk_vec2((float)p->points[i].x, (float)p->points[i].y);
                nk_draw_list_path_line_to(&ctx->draw_list, pnt);
            }
            nk_draw_list_path_stroke(&ctx->draw_list, p->color, NK_STROKE_CLOSED, p->line_thickness);
        } break;
        case NK_COMMAND_POLYGON_FILLED: {
            int i;
            const struct nk_command_polygon_filled *p = (const struct nk_command_polygon_filled*)cmd;
            for (i = 0; i < p->point_count; ++i) {
                struct nk_vec2 pnt = nk_vec2((float)p->points[i].x, (float)p->points[i].y);
                nk_draw_list_path_line_to(&ctx->draw_list, pnt);
            }
            nk_draw_list_path_fill(&ctx->draw_list, p->color);
        } break;
        case NK_COMMAND_POLYLINE: {
            int i;
            const struct nk_command_polyline *p = (const struct nk_command_polyline*)cmd;
            for (i = 0; i < p->point_count; ++i) {
                struct nk_vec2 pnt = nk_vec2((float)p->points[i].x, (float)p->points[i].y);
                nk_draw_list_path_line_to(&ctx->draw_list, pnt);
            }
            nk_draw_list_path_stroke(&ctx->draw_list, p->color, NK_STROKE_OPEN, p->line_thickness);
        } break;
        case NK_COMMAND_RECT_MULTI_COLOR: {
            const struct nk_command_rect_multi_color *r = (const struct nk_command_rect_multi_color*)cmd;
            nk_draw_list_fill_rect_multi_color(&ctx->draw_list, nk_rect(r->x, r->y, r->w, r->h),
                    r->left, r->top, r->right, r->bottom);
        } break; */

        default: break;
        }
    }
    nk_clear(ctx);

    /* delay for remove flickering */
    for (int i = 0; i<100000000000; i++){}
}
