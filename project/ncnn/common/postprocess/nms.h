#pragma once

#include "platform.h"
#include "detection.h"

int nms(const Det* dets, int n, float iou_thr, int* keep, int keep_cap);