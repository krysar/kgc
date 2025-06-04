/*
 * Library for LEDs
*/

#include "hardware/gpio.h"
#include "pico/stdlib.h"

#include "led.h"

bool led_stat[11] = {0};
bool blink_stat = 0;
struct repeating_timer led_blink_tmr;

void led_init() {
    for(register uint8_t i = 2; i <= 14; i++ ) {
        if((i == 5) || (i == 13))
            continue;
        else {
            gpio_init(i);
            gpio_set_dir(i, GPIO_OUT);
        }
    }

    add_repeating_timer_ms(1000, led_blinker, NULL, &led_blink_tmr);
}

void led_update(KSP_DATA ksp_in) {
    // FLD LED:
    if(ksp_in.fld == 1) {
        if(ksp_in.pau == 1) {
            led_stat[LED_STAT_FLD] = true;
        } else {
            led_stat[LED_STAT_FLD] = false;
            gpio_put(LED_FLD, true);
        }
    } else {
        gpio_put(LED_FLD, false);
    }

    // SAS LED:
    (ksp_in.sas == 1) ? gpio_put(LED_SAS, true) : gpio_put(LED_SAS, false);

    // RCS LED:
    (ksp_in.rcs == 1) ? gpio_put(LED_RCS, true) : gpio_put(LED_RCS, false);

    // GEA LED:
    (ksp_in.gea == 1) ? gpio_put(LED_GEA, true) : gpio_put(LED_GEA, false);

    // BRK LED:
    (ksp_in.brk == 1) ? gpio_put(LED_BRK, true) : gpio_put(LED_BRK, false);
}

bool led_blinker(__unused struct repeating_timer *t) {
    if(led_stat[LED_STAT_CON])
        (blink_stat) ? gpio_put(LED_CON, false) : gpio_put(LED_CON, true);
    if(led_stat[LED_STAT_FLD])
        (blink_stat) ? gpio_put(LED_FLD, false) : gpio_put(LED_FLD, true);
    if(led_stat[LED_STAT_ELC])
        (blink_stat) ? gpio_put(LED_ELC, false) : gpio_put(LED_ELC, true);
    if(led_stat[LED_STAT_MOP])
        (blink_stat) ? gpio_put(LED_MOP, false) : gpio_put(LED_MOP, true);
    if(led_stat[LED_STAT_LFO])
        (blink_stat) ? gpio_put(LED_LFO, false) : gpio_put(LED_LFO, true);
    if(led_stat[LED_STAT_OTR])
        (blink_stat) ? gpio_put(LED_OTR, false) : gpio_put(LED_OTR, true);
    if(led_stat[LED_STAT_SAS])
        (blink_stat) ? gpio_put(LED_SAS, false) : gpio_put(LED_SAS, true);
    if(led_stat[LED_STAT_RCS])
        (blink_stat) ? gpio_put(LED_RCS, false) : gpio_put(LED_RCS, true);
    if(led_stat[LED_STAT_GEA])
        (blink_stat) ? gpio_put(LED_GEA, false) : gpio_put(LED_GEA, true);
    if(led_stat[LED_STAT_BRK])
        (blink_stat) ? gpio_put(LED_BRK, false) : gpio_put(LED_BRK, true);
    if(led_stat[LED_STAT_BUZZ])
        (blink_stat) ? gpio_put(BUZZ, false) : gpio_put(BUZZ, true);

    blink_stat = !blink_stat;

    return true;
}