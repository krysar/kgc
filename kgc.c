#include <stdio.h>
#include <math.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "max7219.h"
#include "keypad.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

bool timer_handler(struct repeating_timer *t);
int64_t reg1 = 0, reg2 = 0, reg3 = 0; // displays registers
uint8_t hrs, min, sec, milisec100;

int main() {
    stdio_init_all();
    keypad_init();

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

    secchoice:
    while(true) {
        switch(section) {
            case 0: {
                switch (command) {
                    case 0: //no-op
                        //keypad_status = 0;
                        reg1 = 0;
                        reg2 = 0;
                        reg3 = 0;
                        while(keypad_status == 0) {

                        }
                        goto secchoice;
                    case 1: // stopwatch
                        //keypad_status = 0;
                        reg1 = 0, reg2 = 0, reg3 = 0;
                        milisec100 = 0, sec = 0, min = 0, hrs = 0;
                        //keypad_status = 0;
                        while(keypad_status == 0) {
                            if(milisec100 >= 10) { // 100 ms = 1 sec
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
                        goto secchoice;
                    case 2: //odpocet
                        reg1 = 0, reg2 = 0, reg3 = 0;
                        milisec100 = 10, sec = 60;
                        while (keypad_status == 0) {
                            if(milisec100 == 0) {
                                sec--;
                                if(sec == 0)
                                    sec = 60;
                            }
                            reg2 = sec;
                        }

                        goto secchoice;
                    default:
                        reg1 = 1111;
                        reg2 = 1111;
                        reg3 = 1111;
                }
                break;
            }
            case 1: {
                reg1 = 1111;
                reg2 = 22222;
                reg3 = 333333;
                goto secchoice;
            }
            default:
                reg1 = 4321;
                reg2 = 1234;
                reg3 = 9876;
        }
    }
}

bool timer_handler(struct repeating_timer *t) {
    if((section == 0) && (command == 1))
        milisec100++;
    if((section == 0) && (command == 2))
        milisec100--;
    display_number(0, 1, reg1);
    display_number(1, 0, reg2);
    display_number(1, 1, reg3);
    return true;
}

#pragma clang diagnostic pop