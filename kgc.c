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

void led_init();
bool timer_1s_handler(__unused struct repeating_timer *t);

int main() {
    stdio_init_all();
    // Peripherals init
    keypad_init();
    led_init();
    max7219_init();
    spi_send_data(0, REG_SHUTDOWN, 0x01);
    spi_send_data(1, REG_SHUTDOWN, 0x01);
    // Connection with main computer init
    conn_init();
    // Timer init
    struct repeating_timer timer_1s;
    add_repeating_timer_ms(1000, timer_1s_handler, NULL, &timer_1s);

    // Welcome message
    welcome_message_print();

    display_number(0, 0, 0);

    gpio_set_irq_enabled_with_callback(col[0], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &keypad_irq_handler);
    for(register uint8_t i = 1; i < 4; i++) {
        gpio_set_irq_enabled(col[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }
    
    while(true) {
        switch (verb) {
        case 00:
            clear_display(0, 1);
            clear_display(1, 0);
            clear_display(1, 1);
            break;
        
        case 01:
            switch (noun) {
            case 01:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    if(strlen(uart_str_in) > 0) {
                        int alt = 0, apo = 0, per = 0; // TODO: int64_t
                        sscanf(uart_str_in, " %d;%d;%d", &alt, &apo, &per);
                        //printf("str: %s\n", uart_str_in);
                        //printf("Altitude: %d\nApoapsis: %d\nPeriapsis: %d\n\n", alt, apo, per);
                        //memset(uart_str_in, 0, UART_STR_IN_BUF_SIZE);
                        display_number(0, 1, alt);
                        display_number(1, 0, apo);
                        display_number(1, 1, per);
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
        
        case 02:
            clear_display(0, 1);
            clear_display(1, 0);
            clear_display(1, 1);
            long uart_in_num = 0;

            while (keypad_status != KEY_STAT_PRG_CHANGE) {
                // printf("Str: %s", uart_str_in);
                if(strlen(uart_str_in) > 0) {
                    char *tmp;
                    uart_in_num = strtol(uart_str_in, &tmp, NUM_BASE_DEC);
                    //memset(uart_str_in, 0, UART_STR_IN_BUF_SIZE);
                }
                display_number(0, 1, uart_in_num);
            }
            break;
        
        case 99:
            display_number(0, 1, 1234);
            display_number(1, 0, 5678);
            display_number(1, 1, 9012);

        default:
            break;
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

bool timer_1s_handler(__unused struct repeating_timer *t) {
    char buffer[50];
    printf("Verb: %u Noun: %u\n", verb, noun);
    sprintf(buffer, "V:%u;N:%u\n", verb, noun);
    uart_puts(UART_ID, buffer);

    return true;
}

#pragma clang diagnostic pop