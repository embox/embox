#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <chrono>

#include <drivers/video/fb.h>

using namespace cv;
using namespace std;

static void help(void) {
    printf("\nThis sample demonstrates video capture and display\n"
           "Call:\n"
           "imagecapture [video_file]\n\n");
}

static const char* keys = {
    "{help h||}"
    "{@video_file   | \"sample.mp4\" | source video file   }"
};

static void imdrawfb(Mat& img, struct fb_info *fbi) {
    int w, h;

    if (!fbi) {
        printf("fb0 not found\n");
        return;
    }

    h = min((int) fbi->var.yres, img.rows);
    w = min((int) (fbi->var.bits_per_pixel * fbi->var.xres) / 8, 3 * img.cols);

    for (int y = 0; y < h; y++) {
        const uchar *row = &img.at<uchar>(y, 0);
        for (int x = 0; x < w; x += 3) {
            unsigned rgb888 =
                0xFF000000 |
                unsigned(row[x + 2]) |
                (unsigned(row[x + 1]) << 8) |
                (unsigned(row[x]) << 16);

            ((uint32_t *) fbi->screen_base)[fbi->var.xres * y + x / 3] = rgb888;
        }
    }
}

int main(int argc, const char** argv) {
    CommandLineParser parser(argc, argv, keys);
    if (parser.has("help")) {
        help();
        return 0;
    }

    string video_file = parser.get<String>("@video_file");

    VideoCapture cap;
    bool isOpened = cap.open(video_file, CAP_FFMPEG);
    
    if (!isOpened) {
        printf("Cannot open video file: %s\n", video_file.c_str());
        return -1;
    }

    printf("Video file opened successfully\n");

    Mat frame, scaled_frame;
    double fps = cap.get(CAP_PROP_FPS);
    double frame_delay = 1000.0 / fps;

    struct fb_info *fbi = fb_lookup(0);
    if (!fbi) {
        printf("fb0 not found\n");
        return -1;
    }

    while (cap.read(frame)) {
        if (frame.empty()) {
            printf("End of video\n");
            break;
        }

        double scale_factor = min(static_cast<double>(fbi->var.xres) / frame.cols,
                                  static_cast<double>(fbi->var.yres) / frame.rows);
        
        Size scaled_size(frame.cols * scale_factor, frame.rows * scale_factor);
        resize(frame, scaled_frame, scaled_size, 0, 0, INTER_LINEAR);

        auto start_time = chrono::steady_clock::now();
        
        imdrawfb(scaled_frame, fbi);
        
        auto end_time = chrono::steady_clock::now();
        auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        
        if (elapsed_time < frame_delay) {
            usleep((frame_delay - elapsed_time) * 1000);
        }
    }

    // Clear the framebuffer after video playback
    struct fb_fillrect rect;
    rect.dx = 0;
    rect.dy = 0;
    rect.width = fbi->var.xres;
    rect.height = fbi->var.yres;
    rect.color = 0; // Black color
    rect.rop = ROP_COPY;
    fb_fillrect(fbi, &rect);

return 0;
}    