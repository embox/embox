/**
 * @file
 *
 * @date   12.04.2021
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <drivers/video/fb.h>
#include <algorithm>
#include <cv_embox_imshowfb.hpp>

void imshowfb(cv::Mat& img, int fbx) {
    struct fb_info *fbi;
    int w, h;

    fbi = fb_lookup(fbx);
    if (!fbi) {
		fprintf(stderr, "%s: fb%d not found\n", __func__, fbx);

        return;
    }

    h = std::min((int) fbi->var.yres, img.rows);
    w = std::min((int) (fbi->var.bits_per_pixel * fbi->var.xres) / 8, 3 * img.cols);

    for (int y = 0; y < h; y++) {
        const uchar *row = &img.at<uchar>(y, 0);
        for (int x = 0; x < w; x += 3) {
            unsigned rgb888    =
                0xFF000000 |
                unsigned(row[x + 2]) |
                (unsigned(row[x + 1]) << 8) |
                (unsigned(row[x]) << 16);

            ((uint32_t *) fbi->screen_base)[fbi->var.xres * y + x / 3] = rgb888;
        }
    }
}
