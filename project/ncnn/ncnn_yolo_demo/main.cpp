#include <stdio.h>
#include <stdlib.h>
#include "image_io.h"
#include "yolov8n_model.h"
#include "detections_printer.h"


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

    print_detections(dets);

    return 0;
}