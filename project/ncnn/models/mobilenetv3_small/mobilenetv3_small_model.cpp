#include "mobilenetv3_small_model.h"

#include <cstdio>
#include <math.h>
#include <string.h>

#include "platform.h"
#include "mat.h"
#include "net.h"
#include "datareader.h"

#include "assets/mobilenetv3_small_ncnn_param.h"
#include "assets/mobilenetv3_small_ncnn_bin.h"

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

    std::vector<unsigned char> param_buf;
    
    param_buf.resize(mobilenetv3_small_ncnn_param_len + 1);
    memcpy(param_buf.data(), mobilenetv3_small_ncnn_param, mobilenetv3_small_ncnn_param_len);
    param_buf[mobilenetv3_small_ncnn_param_len] = 0;
    printf("[3] load_param\n");

    std::vector<unsigned char> model_buf;
    model_buf.resize(mobilenetv3_small_ncnn_bin_len);
    memcpy(model_buf.data(), mobilenetv3_small_ncnn_bin, mobilenetv3_small_ncnn_bin_len);
    printf("[4] load_model\n");

    const unsigned char* param_mem = param_buf.data();
    const unsigned char* model_mem = model_buf.data();
    printf("[5] make input\n");

    ncnn::DataReaderFromMemory param_dr(param_mem);
    printf("[6] ex.input\n");
    ncnn::DataReaderFromMemory model_dr(model_mem);
    printf("[7] ex.extract\n");

    if (net.load_param(param_dr) != 0) {
        return false;
    }
    if (net.load_model(model_dr) != 0) {
        return false;
    }

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(
        rgb, ncnn::Mat::PIXEL_RGB, w, h, cfg.input_size, cfg.input_size
    );

    const float norm[3] = {1.f / 255.f, 1.f / 255.f, 1.f / 255.f};
    in.substract_mean_normalize(0, norm);

    ncnn::Extractor ex = net.create_extractor();
    ex.set_light_mode(true);

    if (ex.input("in0", in) != 0) {
        return false;
    }

    ncnn::Mat prob;
    if (ex.extract("out0", prob) != 0) {
        return false;
    }

    const int count = prob.w * prob.h * prob.c;
    if (count <= 0) {
        return false;
    }

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