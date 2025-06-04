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

    gpio_put(LED_CON, true);
    
    while(true) {
        KSP_DATA flight_data = uart_data_decoder(uart_str_in);

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
            if((noun > 0) && (noun <= 12)) {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_number(1, 1, flight_data.num3);
                }
            } else {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
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
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_number(1, 1, flight_data.num3);
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