#include <stdio.h>
#include <stdlib.h>

#include "image_io.h"
#include "mobilenetv3_small_model.h"

int main(int argc, char** argv) {
    const char* img = (argc > 1) ? argv[1] : "/data/photos/dog.png";

    std::vector<unsigned char> rgb;
    int w = 0, h = 0;

    printf("[1] img=%s\n", img);
    if (!load_image_rgb(img, rgb, w, h)) {
        printf("[1E] load_image_rgb failed\n");
        return -2;
    }

    std::vector<ClassProb> out;
    const MobileNetV3SmallConfig& cfg = mobilenetv3_small_default_config();

    if (!mobilenetv3_small_classify_rgb(rgb.data(), w, h, out, cfg)) {
        printf("[2E] mobilenetv3_small_classify_rgb failed\n");
        return -3;
    }

    printf("[2] topk=%d\n", (int)out.size());
    for (size_t i = 0; i < out.size(); ++i) {
        printf("cls=%d prob=%.3f\n", out[i].cls, out[i].prob);
    }

    return 0;
}