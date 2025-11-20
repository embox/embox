#include <stdio.h>
#include "net.h"

int main(int argc, char **argv) {
    printf("ncnn demo: start\n");

    ncnn::Net net;
    // если будешь грузить из памяти:
    // net.load_param(mem_param);
    // net.load_model(mem_bin);

    printf("ncnn demo: ok\n");
    return 0;
}
