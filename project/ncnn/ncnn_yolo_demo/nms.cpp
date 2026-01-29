#include "nms.h"

static float iou(const Det& a, const Det& b){
    float x1 = std::max(a.x, b.x), y1 = std::max(a.y, b.y);
    float x2 = std::min(a.x+a.w, b.x+b.w), y2 = std::min(a.y+a.h, b.y+b.h);
    float inter = std::max(0.f, x2-x1) * std::max(0.f, y2-y1);
    float u = a.w*a.h + b.w*b.h - inter;
    return u>0 ? inter/u : 0.f;
}

int nms(Det* d, int n, float thr, int* keep, int cap){
    int m = 0;
    for (int i=0;i<n && m<cap;++i){
        bool ok = true;
        for (int j=0;j<m;++j){
            const Det& k = d[keep[j]];
            if (d[i].cls != k.cls) continue; 
            if (iou(d[i], d[keep[j]]) > thr){ ok=false; break; }
        }
        if (ok) keep[m++] = i;
    }
    return m;
}