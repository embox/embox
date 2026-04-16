#include <stdio.h>
#include <stdlib.h>

#include "../common/image/image_io.h"
#include "../models/yolov8n/yolov8n_model.h"


static const char* COCO80[80] = {
 "person","bicycle","car","motorcycle","airplane","bus","train","truck","boat","traffic light",
 "fire hydrant","stop sign","parking meter","bench","bird","cat","dog","horse","sheep","cow",
 "elephant","bear","zebra","giraffe","backpack","umbrella","handbag","tie","suitcase","frisbee",
 "skis","snowboard","sports ball","kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket","bottle",
 "wine glass","cup","fork","knife","spoon","bowl","banana","apple","sandwich","orange",
 "broccoli","carrot","hot dog","pizza","donut","cake","chair","couch","potted plant","bed",
 "dining table","toilet","tv","laptop","mouse","remote","keyboard","cell phone","microwave","oven",
 "toaster","sink","refrigerator","book","clock","vase","scissors","teddy bear","hair drier","toothbrush"
};

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
    if (!yolov8n_detect_rgb(rgb.data(), w, h, dets, 320, 0.25f, 0.50f, 16)) {
        printf("[2E] yolov8n_detect_rgb failed\n");
        return -3;
    }

    printf("[2] detections=%d\n", (int)dets.size());
    for (size_t i = 0; i < dets.size(); ++i) {
        const Det& d = dets[i];
        const char* name = (d.cls >= 0 && d.cls < 80) ? COCO80[d.cls] : "cls";
        printf("%s cls=%d conf=%.3f x=%.1f y=%.1f w=%.1f h=%.1f\n",
               name, d.cls, d.score, d.x, d.y, d.w, d.h);
    }

    return 0;
}