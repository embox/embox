#include<stdio.h>
#include <plc/modbus_plc.h>

int main(int argc, char **argv) {
    server_settings_t *settings = get_server_settings();
    printf("%s:%d\n", settings->host, settings->port);
    free(settings);
}