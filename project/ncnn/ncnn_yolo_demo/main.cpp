#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "platform.h"
// #include <algorithm>
// #include <cmath>

#include "mat.h"
#include "net.h"
#include "datareader.h"

#include "yolov8n_ncnn_model/yolov8n.id.h"
#include "yolov8n_ncnn_model/yolov8n.mem.h"

#include "dfl_decode.h"
#include "nms.h"
#include "image_io.h"

static inline int iround(float x) {
    return (int)(x + (x >= 0.0f ? 0.5f : -0.5f));
}

// -------------------- PPM loader (P6/P3) --------------------
static int _ppm_read_token(FILE* f, char* buf, int cap) {
    int c;
    do {
        c = fgetc(f);
        if (c == '#') {
            while ((c = fgetc(f)) != '\n' && c != EOF) {}
        }
    } while (c != EOF && isspace(c));

    if (c == EOF) return 0;

    int n = 0;
    buf[n++] = (char)c;

    while (n < cap - 1) {
        c = fgetc(f);
        if (c == EOF || isspace(c) || c == '#') {
            if (c == '#') {
                while ((c = fgetc(f)) != '\n' && c != EOF) {}
            }
            break;
        }
        buf[n++] = (char)c;
    }

    buf[n] = 0;
    return n;
}

static bool load_ppm_rgb(const char* path, std::vector<unsigned char>& rgb, int& w, int& h) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        perror(path);
        return false;
    }

    char tok[64];
    if (!_ppm_read_token(f, tok, sizeof(tok)) || tok[0] != 'P' || (tok[1] != '6' && tok[1] != '3')) {
        printf("Bad magic in %s: '%s'\n", path, tok);
        return false;
    }
    const int isP6 = (tok[1] == '6');

    if (!_ppm_read_token(f, tok, sizeof(tok))) { fclose(f); return false; }
    w = atoi(tok);
    if (!_ppm_read_token(f, tok, sizeof(tok))) { fclose(f); return false; }
    h = atoi(tok);

    int maxv = 255;
    long pos = ftell(f);
    if (_ppm_read_token(f, tok, sizeof(tok))) {
        int isnum = 1;
        for (char* p = tok; *p; ++p) {
            if (!isdigit((unsigned char)*p)) {
                isnum = 0;
                break;
            }
        }
        if (isnum) maxv = atoi(tok);
        else fseek(f, pos, SEEK_SET);
    }
    if (maxv != 255) printf("Warning: maxval=%d (ожидали 255)\n", maxv);

    if (isP6) {
        int c = fgetc(f);
        if (c != EOF && !isspace(c)) ungetc(c, f);

        rgb.resize((size_t)w * h * 3);
        size_t n = fread(rgb.data(), 1, rgb.size(), f);
        fclose(f);
        if (n != rgb.size()) {
            printf("Short read: %zu/%zu\n", n, (size_t)rgb.size());
            return false;
        }
        return true;
    } else {
        rgb.resize((size_t)w * h * 3);

        for (size_t i = 0; i < rgb.size(); ++i) {
            if (!_ppm_read_token(f, tok, sizeof(tok))) {
                fclose(f);
                return false;
            }
            int v = atoi(tok);
            if (v < 0) v = 0;
            if (v > 255) v = 255;
            rgb[i] = (unsigned char)v;
        }
        fclose(f);
        return true;
    }
}

// -------------------- COCO-80 labels --------------------
static const char* COCO80[80] = {
 "person","bicycle","car","motorcycle","airplane","bus","train","truck","boat","traffic light",
 "fire hydrant","stop sign","parking meter","bench","bird","cat","dog","horse","sheep","cow",
 "elephant","bear","zebra","giraffe","backpack","umbrella","handbag","tie","suitcase","frisbee",
 "skis","snowboard","sports ball","kite","baseball bat","baseball glove","skateboard","surfboard","tennis racket","bottle",
 "wine glass","cup","fork","knife","spoon","bowl","banana","apple","sandwich","orange",
 "broccoli","carrot","hot dog","pizza","donut","cake","chair","couch","potted plant","bed",
 "dining table","toilet","tv","laptop","mouse","remote","keyboard","cell phone","microwave","oven",
 "toaster","sink","refrigerator","book","clock","vase","scissors","teddy bear","hair drier","toothbrush"
};

static void draw_rect_rgb(unsigned char* rgb, int W, int H, int x, int y, int w, int h, int t=2) {
    #define PIX(xx,yy) (rgb + ((yy)*(W) + (xx))*3)
    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + w - 1; if (x1 >= W) x1 = W - 1;
    int y1 = y + h - 1; if (y1 >= H) y1 = H - 1;
    if (x0 > x1 || y0 > y1) return;

    for (int k = 0; k < t; ++k) {
        for (int xx = x0; xx <= x1; ++xx) {
            unsigned char* p = PIX(xx, y0 + k);
            p[0] = 255; p[1] = 0; p[2] = 0;
        }
        for (int xx = x0; xx <= x1; ++xx) {
            unsigned char* p = PIX(xx, y1 - k);
            p[0] = 255; p[1] = 0; p[2] = 0;
        }
        for (int yy = y0; yy <= y1; ++yy) {
            unsigned char* p = PIX(x0 + k, yy);
            p[0] = 255; p[1] = 0; p[2] = 0;
        }
        for (int yy = y0; yy <= y1; ++yy) {
            unsigned char* p = PIX(x1 - k, yy);
            p[0] = 255; p[1] = 0; p[2] = 0;
        }
    }
    #undef PIX
}

static bool save_ppm_rgb(const char* path, const unsigned char* rgb, int w, int h) {
    FILE* f = fopen(path, "wb");
    if (!f) return false;
    printf("P6\n%d %d\n255\n", w, h);
    size_t n = fwrite(rgb, 1, (size_t)w*h*3, f);
    return n == (size_t)w*h*3;
}

struct LetterboxInfo { int nw, nh, pad_x, pad_y; float scale; };


static void log_mat(const char* tag, const ncnn::Mat& m)
{
    printf("[%s] Mat: w=%d h=%d c=%d dims=%d elemsize=%zu elempack=%d cstep=%zu data=%p\n",
           tag, m.w, m.h, m.c, m.dims, (size_t)m.elemsize, m.elempack, (size_t)m.cstep, m.data);
}

static void log_letterbox(const LetterboxInfo& L, int S)
{
    printf("[LB] S=%d nw=%d nh=%d pad_x=%d pad_y=%d scale=%.6f\n",
           S, L.nw, L.nh, L.pad_x, L.pad_y, (double)L.scale);
}

static void log_dets_head(const std::vector<Det>& dets, int n=5)
{
    printf("[DETS] count=%zu\n", dets.size());
    for (int i = 0; i < n && i < (int)dets.size(); ++i)
    {
        const Det& d = dets[i];
        printf("  #%d cls=%d score=%.5f x=%.2f y=%.2f w=%.2f h=%.2f\n",
               i, d.cls, d.score, d.x, d.y, d.w, d.h);
    }
}


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
    lb.resize((size_t)S * S * 3);
    for (size_t i = 0; i < lb.size(); ++i) {
        lb[i] = (unsigned char)114;
    }

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


static inline void clip_box(Det& d, int W, int H) {
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

int main(int argc, char** argv) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    const char* img = (argc > 1) ? argv[1] : "/data/photos/dog.png";
    const int   S   = 320;
    const float CONF = 0.25f;
    const float IOU  = 0.50f;
    const int   REG_MAX = 16;

    // [1] image
    std::vector<unsigned char> rgb;
    int w0 = 0, h0 = 0;
    printf("[1] img=%s S=%d\n", img, S);
    if (!load_image_rgb(img, rgb, w0, h0)) {
        printf("[1E] load_image_rgb failed\n");
        return -2;
    }
    printf("[1] ok: %dx%d bytes=%zu\n", w0, h0, rgb.size());

    // [2] model load
    printf("[2] load model...\n");
    ncnn::Net net;
    net.opt.use_vulkan_compute = false;
    net.opt.num_threads = 1;
    net.opt.lightmode = true;
    net.opt.use_winograd_convolution = false;
    net.opt.use_sgemm_convolution = false;
    net.opt.use_packing_layout = false;


    const unsigned char* pparam = model_opt_param_bin;
    const unsigned char* pmodel = model_opt_bin;
    printf("[2] ptrs: pparam=%p pmodel=%p\n", (const void*)pparam, (const void*)pmodel);

    ncnn::DataReaderFromMemory pr(pparam);
    ncnn::DataReaderFromMemory mr(pmodel);

    int r1 = net.load_param_bin(pr);
    int r2 = (r1 == 0) ? net.load_model(mr) : -999;
    printf("[2] rc: param=%d model=%d\n", r1, r2);
    if (r1 != 0 || r2 != 0) return -3;

    // [3] preprocess
    printf("[3] preprocess(letterbox+norm)...\n");
    LetterboxInfo LI;
    std::vector<unsigned char> lb = make_letterbox_rgb(rgb.data(), w0, h0, S, LI);
    printf("[3] lb bytes=%zu nw=%d nh=%d pad=(%d,%d) scale=%.6f\n",
           lb.size(), LI.nw, LI.nh, LI.pad_x, LI.pad_y, (double)LI.scale);

    ncnn::Mat in = ncnn::Mat::from_pixels(lb.data(), ncnn::Mat::PIXEL_RGB, S, S);
    const float norm[3] = {1.f/255.f, 1.f/255.f, 1.f/255.f};
    in.substract_mean_normalize(0, norm);
    printf("[3] in mat: w=%d h=%d c=%d dims=%d data=%p\n", in.w, in.h, in.c, in.dims, in.data);

    // [4] inference
    printf("[4] infer(input+extract)...\n");
    ncnn::Extractor ex = net.create_extractor();
    ex.set_light_mode(true);

    using namespace model_opt_param_id;
    int ri = ex.input(BLOB_in0, in);
    ncnn::Mat out;
    int ro = (ri == 0) ? ex.extract(BLOB_out0, out) : -999;
    printf("[4] rc: input=%d extract=%d out(w=%d h=%d c=%d dims=%d data=%p)\n",
           ri, ro, out.w, out.h, out.c, out.dims, out.data);
    if (ri != 0 || ro != 0) return -6;

    // [5] postprocess
    printf("[5] decode+nms...\n");
    std::vector<Det> dets;
    const int C = out.h;

    if (C > 4 * REG_MAX) yolo_decode_xywh_flat(out, CONF, dets);
    else                yolo_dfl_decode_flat(out, S, REG_MAX, CONF, dets);

    printf("[5] dets=%zu (C=%d)\n", dets.size(), C);
    if (dets.empty()) return 0;

    for (size_t i = 0; i < dets.size(); ++i) {
        const Det& d = dets[i];
        const char* name = (d.cls >= 0 && d.cls < 80) ? COCO80[d.cls] : "cls";
        printf("raw[%zu] %s cls=%d conf=%.3f x=%.1f y=%.1f w=%.1f h=%.1f\n",
            i, name, d.cls, d.score, d.x, d.y, d.w, d.h);
    }

    sort_dets_by_score_desc(dets);
    for (auto& d : dets) clip_box(d, S, S);

    std::vector<int> keep(dets.size());
    int k = nms(dets.data(), (int)dets.size(), IOU, keep.data(), (int)keep.size());
    printf("[5] nms keep=%d\n", k);

    for (int i = 0; i < k && i < 10; ++i) {
        const Det& d = dets[keep[i]];
        const char* name = (d.cls >= 0 && d.cls < 80) ? COCO80[d.cls] : "cls";
        printf("  %s cls=%d conf=%.3f x=%.1f y=%.1f w=%.1f h=%.1f\n",
               name, d.cls, d.score, d.x, d.y, d.w, d.h);
    }

    return 0;
}
