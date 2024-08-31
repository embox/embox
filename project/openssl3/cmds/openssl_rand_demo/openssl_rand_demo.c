#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/rand.h>

int main(int argc, char **argv) {
    int outf;
    unsigned char buf[5] = { 'a', 'b', 'c', 'd', 'e' };

    if (RAND_bytes(buf, sizeof(buf))) { /* 1 success, 0 otherwise */
        outf = open("/mnt/rnd_bytes", O_CREAT|O_TRUNC|O_RDWR, 0600);
        if (outf < 0) {
            perror("open");
            return 1;
        }
        if (write(outf, buf, sizeof(buf)) < 0) {
            perror("write");
            close(outf);
            return 1;
        }
        close(outf);
    } else {
        printf("-ERR: RAND_bytes\n");
    }
    return 0;
}
