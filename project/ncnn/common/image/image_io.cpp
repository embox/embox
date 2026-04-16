#include "image_io.h"
#include <stddef.h>

#include "stb_image.h"

bool load_image_rgb(const char* path,
                    std::vector<unsigned char>& rgb,
                    int& w, int& h)
{
    int n = 0;
    unsigned char* data = stbi_load(path, &w, &h, &n, 3);
    if (!data || w <= 0 || h <= 0) {
        if (data) stbi_image_free(data);
        return false;
    }

    const size_t nbytes = (size_t)w * (size_t)h * 3u;

    rgb.resize(nbytes);
    memcpy(rgb.data(), data, nbytes);

    stbi_image_free(data);
    return true;
}
