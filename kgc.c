#include <stdio.h>
#include <math.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "hardware/irq.h"

#include "max7219.h"
#include "keypad.h"

#define LED_CON 2
#define LED_FLD 3
#define LED_ELC 7
#define LED_MOP 9
#define LED_LFO 8
#define LED_OTR 10
#define LED_SAS 6
#define LED_RCS 11
#define LED_GEA 4
#define LED_BRK 12
#define BUZZ    14

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

int64_t reg1 = 0, reg2 = 0, reg3 = 0; // displays registers
uint8_t hrs, min, sec, milisec100;

void led_init();
void core1_entry();
bool timer_handler(struct repeating_timer *t);

int main() {
    stdio_init_all();
    keypad_init();
    led_init();
    max7219_init();
    spi_send_data(0, REG_SHUTDOWN, 0x01);
    spi_send_data(1, REG_SHUTDOWN, 0x01);
    display_number(0, 0, 0);


    gpio_set_irq_enabled_with_callback(col[0], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &keypad_irq_handler);
    for(register uint8_t i = 1; i < 4; i++) {
        gpio_set_irq_enabled(col[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }

    struct repeating_timer timer;
    add_repeating_timer_ms(100, timer_handler, NULL, &timer);

    multicore_launch_core1(core1_entry);

    while(true) {
        switch(verb) {
            case 0:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);

                while (keypad_status == 0) {

                }

                break;

            case 1:
                while (keypad_status == 0) {

                }

                break;

            case 99: //device test
                reg1 = 0;
                reg2 = 0;
                reg3 = 0;

                for(register uint8_t i = 2; i <= 14; i++ ) {
                    if((i == 5) || (i == 13))
                        continue;
                    else {
                        gpio_put(i, true);
                    }
                }

                uint16_t i = 0;
                while (keypad_status == 0) {
                    display_number(0, 1, i);
                    display_number(1, 0, i);
                    display_number(1, 1, i);

                    i = (i == 9999) ? 0 : i + 1111;

                    sleep_ms(1000);
                }

                for(register uint8_t i = 2; i <= 14; i++ ) {
                    if((i == 5) || (i == 13))
                        continue;
                    else {
                        gpio_put(i, false);
                    }
                }

                break;

            default:
                while (keypad_status == 0) {

                }
        }
    }
}

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

void core1_entry() {
    multicore_fifo_clear_irq();

    while(true) {
        if((verb == 0) || (verb == 99))
            continue;
        else {
            switch (noun) {
                case 0: //no-op
                    reg1 = 0;
                    reg2 = 0;
                    reg3 = 0;
                    while (keypad_status == 0) {

                    }
                    break;
                case 1: // stopwatch
                    reg1 = 0, reg2 = 0, reg3 = 0;
                    milisec100 = 0, sec = 0, min = 0, hrs = 0;
                    while (keypad_status == 0) {
                        if (milisec100 >= 10) { // 100 ms = 1 sec
                            sec++;
                            milisec100 = 0;
                            if (sec == 60) {
                                min++;
                                sec = 0;
                                if (min == 60) {
                                    hrs++;
                                    min = 0;
                                }
                            }
                        }
                        reg1 = sec;
                        reg2 = min;
                        reg3 = hrs;
                    }
                    break;
                case 2: //odpocet
                    reg1 = 0, reg2 = 0, reg3 = 0;
                    milisec100 = 10, sec = 60;
                    while (keypad_status == 0) {
                        if (milisec100 == 0) {
                            sec--;
                            if (sec == 0)
                                sec = 60;
                        }
                        reg2 = sec;
                    }
                    break;
                default:
                    reg1 = 4321;
                    reg2 = 1234;
                    reg3 = 9876;
            }
        }
    }
}

bool timer_handler(struct repeating_timer *t) {
    if (noun == 1)
        milisec100++;
    else if (noun == 2)
        milisec100--;

    if(verb == 0)
        return true;
    else if(verb == 99)
        return true;

    display_number(0, 1, reg1);
    display_number(1, 0, reg2);
    display_number(1, 1, reg3);
    return true;
}

#pragma clang diagnostic pop