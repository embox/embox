#pragma once
#include "mat.h"
// #include <vector>
namespace ncnn { class Mat; }

struct Det { float x,y,w,h,score; int cls; };

int yolo_dfl_decode_head(const ncnn::Mat& fm, int stride,
                         int num_classes, int reg_max, float conf_thr,
                         int imgW, int imgH, std::vector<Det>& out);

int yolo_dfl_decode_flat(const ncnn::Mat& out, int S,
                         int reg_max, float conf_thr,
                         std::vector<Det>& dets);

int yolo_decode_xywh_flat(const ncnn::Mat& out, float conf_thr, std::vector<Det>& dets);