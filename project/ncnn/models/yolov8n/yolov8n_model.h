#pragma once

#include "../../common/postprocess/nms.h"

bool yolov8n_detect_rgb(const unsigned char* rgb,
                        int w,
                        int h,
                        std::vector<Det>& dets,
                        int target_size = 320,
                        float conf_thr = 0.25f,
                        float iou_thr = 0.50f,
                        int reg_max = 16);