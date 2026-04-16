#pragma once

#include "../../common/postprocess/nms.h"

struct YoloV8NConfig {
    int target_size;
    float conf_thr;
    float iou_thr;
    int reg_max;
};

bool yolov8n_detect_rgb(const unsigned char* rgb,
                        int w,
                        int h,
                        std::vector<Det>& dets,
                        const YoloV8NConfig& cfg);

const char* yolov8n_class_name(int cls);
const YoloV8NConfig& yolov8n_default_config();