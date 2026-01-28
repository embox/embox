#pragma once
#include "platform.h"
#include "dfl_decode.h"
int nms(Det* d, int n, float iou_thr, int* keep, int cap);
