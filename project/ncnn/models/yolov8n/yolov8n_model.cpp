#include "yolov8n_model.h"

#include <string.h>

#include "platform.h"
#include "mat.h"
#include "net.h"
#include "datareader.h"

#include "assets/yolov8n_ncnn_model/yolov8n.id.h"
#include "assets/yolov8n_ncnn_model/yolov8n.mem.h"

#include "../../common/postprocess/dfl_decode.h"
#include "../../common/postprocess/nms.h"

static inline int iround(float x) {
    return (int)(x + (x >= 0.0f ? 0.5f : -0.5f));
}

struct LetterboxInfo {
    int nw, nh, pad_x, pad_y;
    float scale;
};

static std::vector<unsigned char>
make_letterbox_rgb(const unsigned char* src_rgb, int w, int h, int S, LetterboxInfo& L)
{
    float rw = (float)S / (float)w;
    float rh = (float)S / (float)h;
    float r  = (rw < rh) ? rw : rh;

    L.nw    = iround(w * r);
    L.nh    = iround(h * r);
    L.pad_x = (S - L.nw) / 2;
    L.pad_y = (S - L.nh) / 2;
    L.scale = r;

    ncnn::Mat tmp = ncnn::Mat::from_pixels_resize(src_rgb, ncnn::Mat::PIXEL_RGB,
                                                  w, h, L.nw, L.nh);

    std::vector<unsigned char> resized;
    resized.resize((size_t)L.nw * L.nh * 3);
    tmp.to_pixels(resized.data(), ncnn::Mat::PIXEL_RGB);

    std::vector<unsigned char> lb;
    lb.resize((size_t)S * S * 3, (unsigned char)114);

    for (int y = 0; y < L.nh; ++y) {
        unsigned char*       dst = lb.data() + ((y + L.pad_y) * S + L.pad_x) * 3;
        const unsigned char* src = resized.data() + (size_t)y * L.nw * 3;
        memcpy(dst, src, (size_t)L.nw * 3);
    }

    return lb;
}

static void sort_dets_by_score_desc(std::vector<Det>& dets)
{
    if (dets.empty()) return;

    for (size_t i = 1; i < dets.size(); ++i) {
        Det key = dets[i];
        float key_score = key.score;
        size_t j = i;
        while (j > 0 && dets[j - 1].score < key_score) {
            dets[j] = dets[j - 1];
            --j;
        }
        dets[j] = key;
    }
}

static inline void clip_box(Det& d, int W, int H)
{
    float x1 = d.x;
    float y1 = d.y;
    float x2 = d.x + d.w;
    float y2 = d.y + d.h;

    if (x1 < 0.f) x1 = 0.f;
    if (y1 < 0.f) y1 = 0.f;
    if (x2 > (float)(W - 1)) x2 = (float)(W - 1);
    if (y2 > (float)(H - 1)) y2 = (float)(H - 1);

    d.x = x1;
    d.y = y1;
    d.w = x2 - x1;
    d.h = y2 - y1;
    if (d.w < 0.f) d.w = 0.f;
    if (d.h < 0.f) d.h = 0.f;
}

bool yolov8n_detect_rgb(const unsigned char* rgb,
                        int w,
                        int h,
                        std::vector<Det>& dets,
                        int target_size,
                        float conf_thr,
                        float iou_thr,
                        int reg_max)
{
    dets.clear();

    if (!rgb || w <= 0 || h <= 0) {
        return false;
    }

    ncnn::Net net;
    net.opt.use_vulkan_compute = false;
    net.opt.num_threads = 1;
    net.opt.lightmode = true;
    net.opt.use_winograd_convolution = false;
    net.opt.use_sgemm_convolution = false;
    net.opt.use_packing_layout = false;

    const unsigned char* pparam = model_opt_param_bin;
    const unsigned char* pmodel = model_opt_bin;

    ncnn::DataReaderFromMemory pr(pparam);
    ncnn::DataReaderFromMemory mr(pmodel);

    if (net.load_param_bin(pr) != 0) {
        return false;
    }
    if (net.load_model(mr) != 0) {
        return false;
    }

    LetterboxInfo LI;
    std::vector<unsigned char> lb = make_letterbox_rgb(rgb, w, h, target_size, LI);

    ncnn::Mat in = ncnn::Mat::from_pixels(lb.data(), ncnn::Mat::PIXEL_RGB, target_size, target_size);
    const float norm[3] = {1.f / 255.f, 1.f / 255.f, 1.f / 255.f};
    in.substract_mean_normalize(0, norm);

    ncnn::Extractor ex = net.create_extractor();
    ex.set_light_mode(true);

    using namespace model_opt_param_id;

    if (ex.input(BLOB_in0, in) != 0) {
        return false;
    }

    ncnn::Mat out;
    if (ex.extract(BLOB_out0, out) != 0) {
        return false;
    }

    const int C = out.h;
    if (C > 4 * reg_max) {
        yolo_decode_xywh_flat(out, conf_thr, dets);
    } else {
        yolo_dfl_decode_flat(out, target_size, reg_max, conf_thr, dets);
    }

    if (dets.empty()) {
        return true;
    }

    sort_dets_by_score_desc(dets);
    for (size_t i = 0; i < dets.size(); ++i) {
        clip_box(dets[i], target_size, target_size);
    }

    std::vector<int> keep(dets.size());
    int k = nms(dets.data(), (int)dets.size(), iou_thr, keep.data(), (int)keep.size());

    if (k < 0) {
        dets.clear();
        return false;
    }

    for (int i = 0; i < k; ++i) {
        dets[i] = dets[keep[i]];
    }
    dets.resize(k);

    return true;
}
