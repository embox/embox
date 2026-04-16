#include "dfl_decode.h"
#include "mat.h"
// #include <algorithm>

static inline float sigmoid(float x){ return 1.f/(1.f+expf(-x)); }

int yolo_dfl_decode_head(const ncnn::Mat& fm, int stride,
                         int num_classes, int reg_max, float conf_thr,
                         int imgW, int imgH, std::vector<Det>& out)
{
    if (fm.empty()) return 0;
    const int W = fm.w, H = fm.h;
    const int plane = W * H;
    const float* base = (const float*)fm.data;

    int n_before = (int)out.size();

    for (int iy=0; iy<H; ++iy){
        for (int ix=0; ix<W; ++ix){
            const int idx = iy*W + ix;

            int best_c = -1; float best_p = 0.f;
            for (int c=0; c<num_classes; ++c){
                float p = sigmoid(base[(4*reg_max + c)*plane + idx]);
                if (p > best_p){ best_p = p; best_c = c; }
            }
            if (best_p < conf_thr) continue;

            float ltbr[4];
            for (int s=0; s<4; ++s){
                const int ch0 = s*reg_max;
                float m = base[(ch0+0)*plane + idx];
                for (int i=1;i<reg_max;++i){
                    float v = base[(ch0+i)*plane + idx];
                    if (v > m) m = v;
                }
                float sum = 0.f, ex = 0.f;
                for (int i=0;i<reg_max;++i){
                    float v = expf(base[(ch0+i)*plane + idx] - m);
                    sum += v; ex += v * i;
                }
                ltbr[s] = (ex / sum) * stride;
            }

            float cx = (ix + 0.5f) * stride;
            float cy = (iy + 0.5f) * stride;
            float l = ltbr[0], t = ltbr[1], r = ltbr[2], b = ltbr[3];

            out.push_back(Det{cx - l, cy - t, l + r, t + b, best_p, best_c});
        }
    }
    return (int)out.size() - n_before;
}


static inline float _sigmoid(float x){ return 1.f/(1.f+expf(-x)); }

int yolo_dfl_decode_flat(const ncnn::Mat& out, int S,
                         int reg_max, float conf_thr,
                         std::vector<Det>& dets)
{
    if (out.empty()) return 0;

    const int N = out.w;
    const int C = out.h;
    const int NUMC = C - 4 * reg_max;
    if (NUMC <= 0) return 0;

    const float* base = (const float*)out.data;
    auto at = [&](int k, int idx)->float { return base[k * N + idx]; };

    const int cand[2] = {0, 4 * reg_max};

    auto avg_top_prob = [&](int off)->float {
        const int M = std::min(N, 512);
        float acc = 0.f;
        for (int i = 0; i < M; ++i) {
            float best = 0.f;
            for (int c = 0; c < NUMC; ++c) {
                float v = at(off + c, i);
                float p = (v >= 0.f && v <= 1.f) ? v : _sigmoid(v);
                if (p > best) best = p;
            }
            acc += best;
        }
        return acc / std::max(1, M);
    };

    float a = avg_top_prob(cand[0]);
    float b = avg_top_prob(cand[1]);
    int CLS_OFFSET = (a <= b) ? cand[0] : cand[1];
    int REG_OFFSET = (CLS_OFFSET == 0) ? NUMC : 0;

    bool cls_already_prob = false;
    {
        const int M = std::min(N, 256);
        int in01 = 0;
        for (int i=0;i<M;++i){
            float v = at(CLS_OFFSET, i);
            if (v >= 0.f && v <= 1.f) ++in01;
        }
        cls_already_prob = (in01 > M * 0.9f);
    }

    const int n8  = (S/8 ) * (S/8 );
    const int n16 = (S/16) * (S/16);

    int n_before = (int)dets.size();

    for (int idx = 0; idx < N; ++idx)
    {
        int stride, gw, off;
        if (idx < n8)              { stride=8;  gw=S/8;  off=0; }
        else if (idx < n8+n16)     { stride=16; gw=S/16;  off=n8; }
        else                       { stride=32; gw=S/32;  off=n8+n16; }

        const int local = idx - off;
        const int ix = local % gw;
        const int iy = local / gw;

        int best_c = -1; float best_p = 0.f;
        for (int c = 0; c < NUMC; ++c) {
            float v = at(CLS_OFFSET + c, idx);
            float p = cls_already_prob ? v : _sigmoid(v);
            if (p > best_p) { best_p = p; best_c = c; }
        }
        if (best_p < conf_thr) continue;

        float ltbr[4];
        for (int s = 0; s < 4; ++s) {
            const int ch0 = REG_OFFSET + s * reg_max;
            float m = at(ch0, idx);
            for (int i=1;i<reg_max;++i) m = std::max(m, at(ch0+i, idx));
            float denom = 0.f, ex = 0.f;
            for (int i=0;i<reg_max;++i) {
                float e = expf(at(ch0+i, idx) - m);
                denom += e; ex += e * i;
            }
            ltbr[s] = (ex / std::max(denom, 1e-12f)) * stride;
        }

        const float cx = (ix + 0.5f) * stride;
        const float cy = (iy + 0.5f) * stride;

        dets.push_back(Det{
            cx - ltbr[0], cy - ltbr[1],
            ltbr[0] + ltbr[2], ltbr[1] + ltbr[3],
            best_p, best_c
        });
    }
    return (int)dets.size() - n_before;
}



int yolo_decode_xywh_flat(const ncnn::Mat& out, float conf_thr, std::vector<Det>& dets)
{
    if (out.empty()) return 0;
    const int N = out.w;
    const int C = out.h;
    if (C < 5) return 0;

    const int NUMC = C - 4;
    const float* base = (const float*)out.data;
    auto at = [&](int ch, int idx)->float { return base[ch * N + idx]; };

    int added = 0;
    for (int idx = 0; idx < N; ++idx)
    {
        float cx = at(0, idx);
        float cy = at(1, idx);
        float w  = at(2, idx);
        float h  = at(3, idx);

        int best_c = -1; float best_p = 0.f;
        for (int c = 0; c < NUMC; ++c){
            float p = at(4 + c, idx);
            if (p > best_p){ best_p = p; best_c = c; }
        }
        if (best_p < conf_thr) continue;

        float x = cx - 0.5f * w;
        float y = cy - 0.5f * h;

        dets.push_back(Det{ x, y, w, h, best_p, best_c });
        ++added;
    }
    return added;
}