#define __STDCPP_WANT_MATH_SPEC_FUNCS__ 1

#include <iostream>
#include <fstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

int main() {
    const char* filename = "test.mp4";

    // Check if the file exists
    std::ifstream file(filename);
    if (!file.good()) {
        std::cout << "File does not exist: " << filename << std::endl;
        return -1;
    }
    file.close();

    avformat_network_init();

    AVFormatContext* pFormatContext = nullptr;
    int ret = avformat_open_input(&pFormatContext, filename, nullptr, nullptr);
    if (ret < 0) {
        char errbuf[256];
        av_strerror(ret, errbuf, sizeof(errbuf));
        std::cout << "Error opening video file: " << errbuf << std::endl;
        return -1;
    }

    if (avformat_find_stream_info(pFormatContext, nullptr) < 0) {
        std::cout << "Error finding stream information" << std::endl;
        return -1;
    }

    int videoStreamIndex = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videoStreamIndex < 0) {
        std::cout << "No video stream found" << std::endl;
        return -1;
    }

    AVCodecParameters* pCodecParameters = pFormatContext->streams[videoStreamIndex]->codecpar;
    AVCodec* pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == nullptr) {
        std::cout << "Unsupported codec" << std::endl;
        return -1;
    }

    AVCodecContext* pCodecContext = avcodec_alloc_context3(pCodec);
    if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0) {
        std::cout << "Failed to copy codec parameters to codec context" << std::endl;
        return -1;
    }

    if (avcodec_open2(pCodecContext, pCodec, nullptr) < 0) {
        std::cout << "Failed to open codec" << std::endl;
        return -1;
    }

    AVFrame* pFrame = av_frame_alloc();
    if (pFrame == nullptr) {
        std::cout << "Failed to allocate frame" << std::endl;
        return -1;
    }

    AVPacket packet;
    while (av_read_frame(pFormatContext, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(pCodecContext, &packet) < 0) {
                std::cout << "Error sending packet for decoding" << std::endl;
                return -1;
            }

            while (avcodec_receive_frame(pCodecContext, pFrame) == 0) {
                // Process the decoded frame
                std::cout << "Frame decoded: " << pFrame->width << "x" << pFrame->height << std::endl;
            }
        }
        av_packet_unref(&packet);
    }

    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecContext);
    avformat_close_input(&pFormatContext);
    avformat_network_deinit();

    return 0;
}