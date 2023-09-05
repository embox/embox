#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <modbus.h>
#include "errno.h"
#include "string.h"

#define DEFAULT_N 1000
#define DEFAULT_IP "10.0.2.16"
#define N_MAX_FAILS 3

int main(int argc, char **argv) { 
    int p = 0;
    unsigned char tab_bits[8] = {0,0,0,0,0,0,0,0};
    unsigned int N_times = DEFAULT_N;
    unsigned int N_fails = 0;
    char *srv_ip;

    if (argc>1) {
        srv_ip = argv[1];
        if(argc==3) {
            N_times = strtoul(argv[2], NULL, 10);
            if(!N_times){
                printf("Invalid argument, defaulting to N=%d\n", DEFAULT_N);
                N_times = DEFAULT_N;
            }
        }
    }
    else {
        srv_ip = malloc(strlen(DEFAULT_IP));
        memcpy(srv_ip, DEFAULT_IP, strlen(DEFAULT_IP));
    }

    for (int i=0; i<N_times; i++) {
        modbus_t * mb_ = NULL;

        mb_ = modbus_new_tcp(srv_ip, 500);

        if (mb_ == NULL) {
            fprintf(stderr, "Unable to allocate libmodbus context\n");
            exit(EXIT_FAILURE);
        }

        if( modbus_set_response_timeout(mb_, 0, 400000) == -1) {
            fprintf(stderr, "Modbus change response timeout error: %s\n", 
                    modbus_strerror(errno));
            modbus_free(mb_);
            exit(EXIT_FAILURE);
        }

        if (modbus_connect(mb_) == -1) {
            fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
            modbus_free(mb_);
            N_fails++;
            if (N_fails==N_MAX_FAILS) {
                printf("Exiting..too many failures: %d/%d\n", N_fails, N_MAX_FAILS);
                printf("Failure\n");
                return -1;
            }
            continue;
        }

        modbus_read_bits(mb_, 0, 8, tab_bits);
        modbus_close(mb_);
        modbus_free(mb_);

        printf("Packet number %d bits:\n", p);
        printf("( ");
        for (int k=0; k<8; k++) {
            printf("%d ", tab_bits[k] );
        }
        printf(")\n");
        p=p+1;
    }
    printf("Finished reading. Failed %d/%d\n", N_fails, N_times);
    printf("Success\n");

    return 0;
}
