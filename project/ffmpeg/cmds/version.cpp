#define __STDCPP_WANT_MATH_SPEC_FUNCS__ 1
#include <stdio.h>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

int main() {
    printf("FFmpeg version: %s\n", av_version_info());
    return 0;
}