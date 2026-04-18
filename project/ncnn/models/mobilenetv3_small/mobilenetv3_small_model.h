#pragma once

#include "platform.h"
#include "classification.h"

struct MobileNetV3SmallConfig {
    int input_size;
    int topk;
};

const MobileNetV3SmallConfig& mobilenetv3_small_default_config();

bool mobilenetv3_small_classify_rgb(const unsigned char* rgb,
                                    int w,
                                    int h,
                                    std::vector<ClassProb>& out,
                                    const MobileNetV3SmallConfig& cfg);