#include <stdio.h>
#include <stdlib.h>
#include "coco80.h"
#include "image_io.h"
#include "yolov8n_model.h"


int main(int argc, char** argv) {
    const char* img = (argc > 1) ? argv[1] : "/data/photos/dog.png";

    std::vector<unsigned char> rgb;
    int w = 0, h = 0;

    printf("[1] img=%s\n", img);
    if (!load_image_rgb(img, rgb, w, h)) {
        printf("[1E] load_image_rgb failed\n");
        return -2;
    }

    std::vector<Det> dets;
    const YoloV8NConfig& cfg = yolov8n_default_config();

    if (!yolov8n_detect_rgb(rgb.data(), w, h, dets, cfg)) {
        printf("[2E] yolov8n_detect_rgb failed\n");
        return -3;
    }

    printf("[2] detections=%d\n", (int)dets.size());
    for (size_t i = 0; i < dets.size(); ++i) {
        const Det& d = dets[i];
        const char* name = coco80_class_name(d.cls);
        printf("%s cls=%d conf=%.3f x=%.1f y=%.1f w=%.1f h=%.1f\n",
               name, d.cls, d.score, d.x, d.y, d.w, d.h);
    }

    return 0;
}