#include "mobilenetv3_small_model.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "platform.h"
#include "mat.h"
#include "datareader.h"
#include "net.h"

#include "assets/mobilenetv3_small.id.h"
#include "assets/mobilenetv3_small.mem.h"

static const MobileNetV3SmallConfig kMobileNetV3SmallDefaultConfig = {
    224,
    5
};

const MobileNetV3SmallConfig& mobilenetv3_small_default_config()
{
    return kMobileNetV3SmallDefaultConfig;
}

static void softmax_inplace(std::vector<ClassProb>& out)
{
    if (out.empty()) return;

    float maxv = out[0].prob;
    for (size_t i = 1; i < out.size(); ++i) {
        if (out[i].prob > maxv) maxv = out[i].prob;
    }

    float sum = 0.f;
    for (size_t i = 0; i < out.size(); ++i) {
        out[i].prob = expf(out[i].prob - maxv);
        sum += out[i].prob;
    }

    if (sum > 0.f) {
        for (size_t i = 0; i < out.size(); ++i) {
            out[i].prob /= sum;
        }
    }
}

static void sort_desc(std::vector<ClassProb>& out)
{
    for (size_t i = 1; i < out.size(); ++i) {
        ClassProb key = out[i];
        size_t j = i;
        while (j > 0 && out[j - 1].prob < key.prob) {
            out[j] = out[j - 1];
            --j;
        }
        out[j] = key;
    }
}

bool mobilenetv3_small_classify_rgb(const unsigned char* rgb,
                                    int w,
                                    int h,
                                    std::vector<ClassProb>& out,
                                    const MobileNetV3SmallConfig& cfg)
{
    out.clear();

    if (!rgb || w <= 0 || h <= 0) {
        return false;
    }

    ncnn::Net net;
    net.opt.use_vulkan_compute = false;
    net.opt.num_threads = 1;
    net.opt.lightmode = true;
    net.opt.use_packing_layout = false;

    const unsigned char* param_bin_mem = mobilenet_v3_small_embedded_ncnn_param_bin;
    ncnn::DataReaderFromMemory param_bin_dr(param_bin_mem);

    if (net.load_param_bin(param_bin_dr) != 0) {
        printf("[E] load_param_bin failed\n");
        return false;
    }
    printf("[OK] load_param_bin\n");

    const unsigned char* model_bin_mem = mobilenet_v3_small_embedded_ncnn_bin;
    ncnn::DataReaderFromMemory model_bin_dr(model_bin_mem);

    if (net.load_model(model_bin_dr) != 0) {
        printf("[E] load_model(DataReader) failed\n");
        return false;
    }
    printf("[OK] load_model\n");

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(
        rgb, ncnn::Mat::PIXEL_RGB, w, h, cfg.input_size, cfg.input_size
    );

    const float norm[3] = {1.f / 255.f, 1.f / 255.f, 1.f / 255.f};
    in.substract_mean_normalize(0, norm);

    ncnn::Extractor ex = net.create_extractor();
    ex.set_light_mode(true);

    if (ex.input(mobilenet_v3_small_embedded_ncnn_param_id::BLOB_in0, in) != 0) {
    printf("[E] ex.input failed\n");
        return false;
    }
    printf("[OK] ex.input\n");

    ncnn::Mat prob;
    if (ex.extract(mobilenet_v3_small_embedded_ncnn_param_id::BLOB_out0, prob) != 0) {
        printf("[E] ex.extract failed\n");
        return false;
    }
    printf("[OK] ex.extract\n");

    printf("[DBG] prob: w=%d h=%d c=%d\n", prob.w, prob.h, prob.c);

    const int count = prob.w * prob.h * prob.c;
    if (count <= 0) {
        printf("[E] empty output\n");
        return false;
    }
    printf("[OK] count=%d\n", count);

    const float* p = (const float*)prob.data;
    out.resize(count);
    for (int i = 0; i < count; ++i) {
        out[i].cls = i;
        out[i].prob = p[i];
    }

    softmax_inplace(out);
    sort_desc(out);

    if ((int)out.size() > cfg.topk) {
        out.resize(cfg.topk);
    }

    return true;
}