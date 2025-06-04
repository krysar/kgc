#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "hardware/irq.h"

#include "max7219.h"
#include "keypad.h"
#include "connection.h"
#include "led.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

bool timer_1s_handler(__unused struct repeating_timer *t);

int main() {
    stdio_init_all();
    // Peripherals init
    keypad_init();
    led_init();
    max7219_init();
    spi_send_data(0, REG_SHUTDOWN, 0x01);
    spi_send_data(1, REG_SHUTDOWN, 0x01);
    // Connection with server init
    conn_init();

    // Welcome message
    welcome_message_print();

    // Try to handshake
    handshake:
    uart_handshake();

    verb = 0, noun = 0;
    display_number(0, 0, 0);

    // Timer init
    struct repeating_timer timer_1s;
    add_repeating_timer_ms(1000, timer_1s_handler, NULL, &timer_1s);

    gpio_set_irq_enabled_with_callback(col[0], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &keypad_irq_handler);
    for(register uint8_t i = 1; i < 4; i++) {
        gpio_set_irq_enabled(col[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }
    
    while(true) {
        switch (verb) {
        case 0:
            switch (noun) {
            // Disconnect from server
            case 1:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);

                cancel_repeating_timer(&timer_1s);
                gpio_put(LED_CON, false);
                memset(uart_str_in, 0, UART_STR_IN_BUF_SIZE);
                goto handshake;

                break;
            
            default:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                break;
            }
            break;
        
        case 1:
            switch (noun) {
            // Altitude above sea level + apoapsis + periapsis
            case 1:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t alt = 0, apo = 0, per = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &alt, &apo, &per);
                        display_number(0, 1, alt);
                        display_number(1, 0, apo);
                        display_number(1, 1, per);
                    }
                }
                break;
            
            // Inclination + eccentricity + mean anomaly
            case 2:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t inc = 0, ecc = 0, ano = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &inc, &ecc, &ano);
                        display_number(0, 1, inc);
                        display_number(1, 0, ecc);
                        display_number(1, 1, ano);
                    }
                }
                break;

            // Time to apoapsis [ss:mm:hhhh]
            case 3:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t sec = 0, min = 0, hour = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &sec, &min, &hour);
                        display_number(0, 1, sec);
                        display_number(1, 0, min);
                        display_number(1, 1, hour);
                    }
                }
                break;
            
            // Time to apoapsis [mm:hh:dddd]
            case 4:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t min = 0, hour = 0, day = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &min, &hour, &day);
                        display_number(0, 1, min);
                        display_number(1, 0, hour);
                        display_number(1, 1, day);
                    }
                }
                break;

            // Time to periapsis [ss:mm:hhhh]
            case 5:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t sec = 0, min = 0, hour = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &sec, &min, &hour);
                        display_number(0, 1, sec);
                        display_number(1, 0, min);
                        display_number(1, 1, hour);
                    }
                }
                break;

            // Time to periapsis [mm:hh:dddd]
            case 6:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t min = 0, hour = 0, day = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &min, &hour, &day);
                        display_number(0, 1, min);
                        display_number(1, 0, hour);
                        display_number(1, 1, day);
                    }
                }
                break;

            // Period [ss:mm:hhhh]
            case 7:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t sec = 0, min = 0, hour = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &sec, &min, &hour);
                        display_number(0, 1, sec);
                        display_number(1, 0, min);
                        display_number(1, 1, hour);
                    }
                }
                break;

            // Period [mm:hh:dddd]
            case 8:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t min = 0, hour = 0, day = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &min, &hour, &day);
                        display_number(0, 1, min);
                        display_number(1, 0, hour);
                        display_number(1, 1, day);
                    }
                }
                break;

            // Time to sphere of influence change [ss:mm:hhhh]
            case 9:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t sec = 0, min = 0, hour = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &sec, &min, &hour);
                        display_number(0, 1, sec);
                        display_number(1, 0, min);
                        display_number(1, 1, hour);
                    }
                }
                break;

            // Time to sphere of influence change [mm:hh:dddd]
            case 10:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t min = 0, hour = 0, day = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &min, &hour, &day);
                        display_number(0, 1, min);
                        display_number(1, 0, hour);
                        display_number(1, 1, day);
                    }
                }
                break;

            // Pitch + heading + roll
            case 11:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t pit = 0, hea = 0, rol = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &hea, &pit, &rol);
                        display_number(0, 1, hea);
                        display_number(1, 0, pit);
                        display_number(1, 1, rol);
                    }
                }
                break;

            // Velocity [x, y, z]
            case 12:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int64_t vx = 0, vy = 0, vz = 0;
                        sscanf(uart_str_in, " %lld;%lld;%lld", &vx, &vy, &vz);
                        display_number(0, 1, vx);
                        display_number(1, 0, vy);
                        display_number(1, 1, vz);
                    }
                }
                break;
            
            default:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                break;
            }
            break;
        
        // Testing verb
        case 99:
        switch (noun) {
            // Display 3 static numbers
            case 1:
                display_number(0, 1, 1234);
                display_number(1, 0, 5678);
                display_number(1, 1, 9012);
                break;
            
            // Display incrementing number from server
            case 2:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                long uart_in_num = 0;

                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        char *tmp;
                        uart_in_num = strtol(uart_str_in, &tmp, NUM_BASE_DEC);
                    }
                    display_number(0, 1, uart_in_num);
                }
                break;
            
            default:
                break;
            }
        default:
            break;
        }
    }
}

bool timer_1s_handler(__unused struct repeating_timer *t) {
    char buffer[50];
    printf("Verb: %u Noun: %u\n", verb, noun);
    sprintf(buffer, "V:%u;N:%u\n", verb, noun);
    uart_puts(UART_ID, buffer);

    return true;
}

#pragma clang diagnostic pop