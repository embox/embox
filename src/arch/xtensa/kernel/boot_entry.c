#include <stdint.h>

#define FCPU 160000000

#define U0TXD_U_ADDR 0x60000818
#define U0TXD_U_PTR ((uint32_t*) U0TXD_U_ADDR)
#define U0TXD_U (*((volatile uint32_t*) U0TXD_U_ADDR))

#define GPIO_OUT_W1TS (*((volatile uint32_t*) 0x60000304))
#define GPIO_OUT_W1TC (*((volatile uint32_t*) 0x60000308))

#define GPIO_ENABLE_W1TS (*((volatile uint32_t*) 0x60000310))
#define GPIO_ENABLE_W1TC (*((volatile uint32_t*) 0x60000314))

static inline void setFunction(uint32_t *controlRegister, uint8_t function)
{
    uint32_t oldValue = *controlRegister;
    uint32_t functionMask = ((function & 0b100) << 6) | ((function & 0b11) << 4);
    *controlRegister = (oldValue & ~(0b100110000)) | functionMask;
}

static inline void gpioEnable(uint8_t pinNumber)
{
    GPIO_ENABLE_W1TS |= 1 << pinNumber;
}

static inline void gpioDisable(uint8_t pinNumber)
{
    GPIO_ENABLE_W1TC |= 1 << pinNumber;
}

static inline void gpioSet(uint8_t pinNumber)
{
    GPIO_OUT_W1TS |= 1 << pinNumber;
}

static inline void gpioClear(uint8_t pinNumber)
{
    GPIO_OUT_W1TC |= 1 << pinNumber;
}

static inline void gpioInit()
{
    U0TXD_U = 0b10000000;
    GPIO_OUT_W1TC = 0xFFFF;
    GPIO_ENABLE_W1TC = 0xFFFF;
}

//Be careful! This function is very imprecise
static inline void delay(uint32_t milliseconds)
{
    uint32_t nops = ((uint64_t) milliseconds * FCPU) / 30000;
    while (nops > 1)
    {
        asm volatile ("nop");
        nops--;
    }
}

void blinky_test()
{
    gpioInit();
    
    setFunction(U0TXD_U_PTR, 0b11);
    
    gpioEnable(1);
    gpioSet(1);
    
    int status = 1;
    while (1)
    {
        delay(500);
        if (status)
        {
            gpioClear(1);
            status = 0;
        }
        else
        {
            gpioSet(1);
            status = 1;
        }
    }
}

void call_user_start()
{
    blinky_test();
}


void die()
{
    while (1)
    {
    }
}
