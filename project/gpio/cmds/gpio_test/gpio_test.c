#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <util/array.h>
#include <drivers/gpio/gpio.h>

struct led_desc {
    int gpio; /**< port */
    int pin; /**< pin mask */
};

static const struct led_desc leds[] = {
#include <gpio_config.inc>
};

#define LEDS_N ARRAY_SIZE(leds)

int led_driver_set(unsigned int nr) {

    if (nr >= 2)     {
        return -EINVAL;
    }
    gpio_set(leds[nr].gpio, leds[nr].pin, GPIO_PIN_HIGH);

    return 0;
}

int led_driver_clear(unsigned int nr) {

    if (nr >= 2) {
        return -EINVAL;
    }

    gpio_set(leds[nr].gpio, leds[nr].pin, GPIO_PIN_LOW);

    return 0;
}

void sleep (int n) {
    for (int i = 0; i < n * 10000; ++i) {
    }
}

int main(int argc, char *argv[]) {
    int i;
    for (i = 0; i < LEDS_N; i++) {
        if (0 != gpio_setup_mode(leds[i].gpio, leds[i].pin,
                                 GPIO_MODE_OUTPUT)) {
            return -EINVAL;
        }
    }

    while (1) {
        led_driver_clear(0);
        led_driver_set(1);
        sleep (1000);
        led_driver_set(0);
        led_driver_clear(1);
        sleep (1000);
    }

    return 0;
}
