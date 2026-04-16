#include <stdio.h>

#include "detections_printer.h"
#include "coco80.h"

void print_detections(const std::vector<Det>& dets)
{
    printf("[2] detections=%d\n", (int)dets.size());

    for (size_t i = 0; i < dets.size(); ++i) {
        const Det& d = dets[i];
        const char* name = coco80_class_name(d.cls);
        printf("%s cls=%d conf=%.3f x=%.1f y=%.1f w=%.1f h=%.1f\n",
               name, d.cls, d.score, d.x, d.y, d.w, d.h);
    }
}
