/**
 * @file
 * @brief
 *
 * @author  Dmitry Kocherygin
 * @date    15.12.2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <lib/libds/array.h>
#include <drivers/gpio/gpio.h>

struct led_desc {
    int gpio; /**< port */
    int pin; /**< pin mask */
};

static const struct led_desc leds[] = {
#include <leds_config.inc>
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

int main(int argc, char *argv[]) {
    int i;
    for (i = 0; i < LEDS_N; i++) {
        if (0 != gpio_setup_mode(leds[i].gpio, leds[i].pin,
                                 GPIO_MODE_OUT)) {
            return -EINVAL;
        }
    }

    while (1) {        
        for (i = 0; i < LEDS_N; i++) {
            led_driver_set(i);
            sleep (1);
            led_driver_clear(i);
            if (LEDS_N == 1) {
                sleep (1);                
            }                
        }          
    }
    return 0;
}
