#include <iostream>
#include <cstdio>
#include <memory>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
    std::string command = "ffmpeg -version";
    try {
        std::string output = exec(command.c_str());
        if (output.find("ffmpeg version") != std::string::npos) {
            std::cout << "FFmpeg is installed. Version info:" << std::endl;
            std::cout << output << std::endl;
        } else {
            std::cout << "FFmpeg is not installed correctly." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error checking FFmpeg: " << e.what() << std::endl;
    }

    return 0;
}
