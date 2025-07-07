#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <embox/unit.h>
#include <drivers/gpio.h>
#include <framework/mod/options.h>
#include "nanomodbus.h"

/* Конфигурация светодиодов Nucleo-F429ZI */
#define LED1_PORT  GPIO_PORT_B
#define LED1_PIN   0   /* PB0 (зелёный) */
#define LED2_PORT  GPIO_PORT_B
#define LED2_PIN   7   /* PB7 (синий) */
#define LED3_PORT  GPIO_PORT_B
#define LED3_PIN   14  /* PB14 (красный) */

/* Modbus адреса для управления */
#define LED1_COIL_ADDR 0
#define LED2_COIL_ADDR 1
#define LED3_COIL_ADDR 2

/* Состояние виртуальных coils */
static nmbs_bitfield server_coils = {0};

/* Инициализация светодиодов */
static void init_leds(void) {
    /* Настройка пинов как выходов */
    gpio_setup_mode(LED1_PORT, 1 << LED1_PIN, GPIO_MODE_OUT);
    gpio_setup_mode(LED2_PORT, 1 << LED2_PIN, GPIO_MODE_OUT);
    gpio_setup_mode(LED3_PORT, 1 << LED3_PIN, GPIO_MODE_OUT);
    
    /* Изначально все выключены */
    gpio_set(LED1_PORT, 1 << LED1_PIN, 0);
    gpio_set(LED2_PORT, 1 << LED2_PIN, 0);
    gpio_set(LED3_PORT, 1 << LED3_PIN, 0);
    
    /* Инициализация виртуальных coils */
    nmbs_bitfield_write(server_coils, LED1_COIL_ADDR, false);
    nmbs_bitfield_write(server_coils, LED2_COIL_ADDR, false);
    nmbs_bitfield_write(server_coils, LED3_COIL_ADDR, false);
}

/* Обработчик записи coils */
static nmbs_error handle_write_multiple_coils(uint16_t address, uint16_t quantity, 
                                     const nmbs_bitfield coils, uint8_t unit_id, void* arg) {
    (void)unit_id;
    (void)arg;

    for (int i = 0; i < quantity; i++) {
        bool value = nmbs_bitfield_read(coils, i);
        uint16_t current_addr = address + i;
        
        /* Обновляем виртуальное состояние */
        nmbs_bitfield_write(server_coils, current_addr, value);
        
        /* Управляем физическими светодиодами */
        switch(current_addr) {
            case LED1_COIL_ADDR:
                gpio_set(LED1_PORT, 1 << LED1_PIN, value ? 1 : 0);
                printf("LED1 set to %s\n", value ? "ON" : "OFF");
                break;
            case LED2_COIL_ADDR:
                gpio_set(LED2_PORT, 1 << LED2_PIN, value ? 1 : 0);
                printf("LED2 set to %s\n", value ? "ON" : "OFF");
                break;
            case LED3_COIL_ADDR:
                gpio_set(LED3_PORT, 1 << LED3_PIN, value ? 1 : 0);
                printf("LED3 set to %s\n", value ? "ON" : "OFF");
                break;
            default:
                return NMBS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
        }
    }
    return NMBS_ERROR_NONE;
}

/* Обработчик чтения coils */
static nmbs_error handle_read_coils(uint16_t address, uint16_t quantity, 
                           nmbs_bitfield coils_out, uint8_t unit_id, void* arg) {
    (void)unit_id;
    (void)arg;

    for (int i = 0; i < quantity; i++) {
        uint16_t current_addr = address + i;
        bool value = nmbs_bitfield_read(server_coils, current_addr);
        nmbs_bitfield_write(coils_out, i, value);
    }
    return NMBS_ERROR_NONE;
}

/* Функции для работы с сокетами */
static int32_t read_fd(uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {
    (void)byte_timeout_ms;
    int fd = (int)(intptr_t)arg;
    return read(fd, buf, count);
}

static int32_t write_fd(const uint8_t* buf, uint16_t count, int32_t byte_timeout_ms, void* arg) {
    (void)byte_timeout_ms;
    int fd = (int)(intptr_t)arg;
    return write(fd, buf, count);
}

/* Создание TCP сервера */
static int create_tcp_server(const char *ip, const char *port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        return -1;
    }

    if (listen(sockfd, 1) < 0) {
        perror("listen");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int main(int argc, char *argv[]) {
    /* Инициализация оборудования */
    init_leds();
    
    /* Настройка Modbus TCP сервера */
    nmbs_platform_conf platform_conf;
    nmbs_platform_conf_create(&platform_conf);
    platform_conf.transport = NMBS_TRANSPORT_TCP;
    platform_conf.read = read_fd;
    platform_conf.write = write_fd;
    platform_conf.arg = NULL;

    /* Регистрация callback-функций */
    nmbs_callbacks callbacks;
    nmbs_callbacks_create(&callbacks);
    callbacks.read_coils = handle_read_coils;
    callbacks.write_multiple_coils = handle_write_multiple_coils;

    /* Создание сервера */
    nmbs_t nmbs;
    nmbs_error err = nmbs_server_create(&nmbs, 0, &platform_conf, &callbacks);
    if (err != NMBS_ERROR_NONE) {
        printf("Modbus server create error: %s\n", nmbs_strerror(err));
        return 1;
    }

    const char *ip = "0.0.0.0";
    const char *port = "502";
    
    int sockfd = create_tcp_server(ip, port);
    if (sockfd < 0) {
        printf("Failed to create TCP server\n");
        return 1;
    }

    printf("Modbus TCP server started at %s:%s\n", ip, port);

    while (1) {
        int client_fd = accept(sockfd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        platform_conf.arg = (void*)(intptr_t)client_fd;
        nmbs_set_platform_arg(&nmbs, platform_conf.arg);

        while (1) {
            err = nmbs_server_poll(&nmbs);
            if (err != NMBS_ERROR_NONE) {
                printf("Modbus error: %s\n", nmbs_strerror(err));
                break;
            }
            usleep(10000); /* 10 ms delay */
        }
        
        close(client_fd);
    }

    close(sockfd);
    return 0;
}