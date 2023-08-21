#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <modbus.h>
#include <fcntl.h>
#include <string.h>

#define QUERY_LEN 10
#define LEDDRV_LED_N 8
#define ON 1
#define OFF 0

static modbus_mapping_t *mb_mapping_wrapper_new(void) {
    modbus_mapping_t *mb_mapping;
    mb_mapping = modbus_mapping_new(LEDDRV_LED_N, 0, LEDDRV_LED_N, 0);

    unsigned char bits_init[8] = {1,0,1,0,1,0,1,0};
    for (int i=0; i<LEDDRV_LED_N; i++) {
        mb_mapping->tab_bits[i] = bits_init[i];
    }

    return mb_mapping;
}

static void mb_mapping_wrapper_free(modbus_mapping_t *mb_mapping) {
    modbus_mapping_free(mb_mapping);
}

static void mb_mapping_shift_state(modbus_mapping_t *mb_mapping) {
    unsigned char last = mb_mapping->tab_bits[LEDDRV_LED_N-1];

    for(int i=LEDDRV_LED_N-1; i>0; i--) {
        mb_mapping->tab_bits[i] = mb_mapping->tab_bits[i-1];
    }
    mb_mapping->tab_bits[0] = last;
}

int main(int argc, char** argv) {
    char ip[] = "0.0.0.0";
    int port = 500;
    modbus_t *ctx;
    int header_len;
    uint8_t *query;
    modbus_mapping_t *mb_mapping;
    int listen_socket, client_socket;

    printf("Modbus test-server starting\n");
    ctx = modbus_new_tcp(ip, port);
    header_len = modbus_get_header_length(ctx);
    query = malloc(MODBUS_TCP_MAX_ADU_LENGTH);

    modbus_set_debug(ctx, FALSE);

    mb_mapping = mb_mapping_wrapper_new();
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    /* backlog=2 is because client may be reconnecting too fast getting
     * into situation where previous socket is in TIMEWAIT state resulting
     * in connection failure */
    listen_socket = modbus_tcp_listen(ctx, 2);
    for (;;) {
        client_socket = modbus_tcp_accept(ctx, &listen_socket);
        if (-1 == client_socket) {
            printf("client_socket = -1\n");
            continue;
        }

        for (;;) {
            int query_len;

            query_len = modbus_receive(ctx, query);
            if (-1 == query_len) {
                /* Connection closed by the client or error */
                break;
            }

            if (query[header_len - 1] != MODBUS_TCP_SLAVE) {
                continue;
            }

            if (-1 == modbus_reply(ctx, query, query_len, mb_mapping)) {
                break;
            }

            mb_mapping_shift_state(mb_mapping);

        }

        close(client_socket);
    }
    printf("exiting: %s\n", modbus_strerror(errno));

    close(listen_socket);
    mb_mapping_wrapper_free(mb_mapping);
    free(query);
    modbus_free(ctx);

    return 0;
}
