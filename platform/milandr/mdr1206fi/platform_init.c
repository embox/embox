

extern void ClockConfig();
extern void UART_Config();
void platform_init(void) {
    ClockConfig();
    UART_Config();
}
