/*
 * Library for LEDs
*/

#include "hardware/gpio.h"
#include "led.h"

void led_init() {
    for(register uint8_t i = 2; i <= 14; i++ ) {
        if((i == 5) || (i == 13))
            continue;
        else {
            gpio_init(i);
            gpio_set_dir(i, GPIO_OUT);
        }
    }
}