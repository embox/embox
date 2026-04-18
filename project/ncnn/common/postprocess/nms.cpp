#include "nms.h"

static inline float maxf(float a, float b) { return a > b ? a : b; }
static inline float minf(float a, float b) { return a < b ? a : b; }

static float iou(const Det& a, const Det& b)
{
    float x1 = maxf(a.x, b.x);
    float y1 = maxf(a.y, b.y);
    float x2 = minf(a.x + a.w, b.x + b.w);
    float y2 = minf(a.y + a.h, b.y + b.h);

    float iw = x2 - x1;
    float ih = y2 - y1;
    if (iw < 0.f) iw = 0.f;
    if (ih < 0.f) ih = 0.f;

    float inter = iw * ih;
    float ua = a.w * a.h + b.w * b.h - inter;
    if (ua <= 0.f) return 0.f;

    return inter / ua;
}

int nms(const Det* dets, int n, float iou_thr, int* keep, int keep_cap)
{
    if (!dets || !keep || n <= 0 || keep_cap <= 0) return 0;

    int kept = 0;
    for (int i = 0; i < n; ++i) {
        int suppressed = 0;
        for (int j = 0; j < kept; ++j) {
            int kj = keep[j];
            if (dets[i].cls == dets[kj].cls && iou(dets[i], dets[kj]) > iou_thr) {
                suppressed = 1;
                break;
            }
        }
        if (!suppressed) {
            if (kept >= keep_cap) break;
            keep[kept++] = i;
        }
    }

    return kept;
}