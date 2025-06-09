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
#include "hardware/watchdog.h"

#include "max7219.h"
#include "keypad.h"
#include "connection.h"
#include "led.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

float insnms[3] = {0};

bool timer_uart_send_handler(__unused struct repeating_timer *t);
void software_reset();

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
    display_two_digit_number(0, 0, 0, 0);
    display_two_digit_number(0, 0, 1, 0);

    // Timer for periodical sending data via UART init
    struct repeating_timer timer_uart_send;
    add_repeating_timer_ms(UART_SEND_PERIOD_MS, timer_uart_send_handler, NULL, &timer_uart_send);

    gpio_set_irq_enabled_with_callback(col[0], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &keypad_irq_handler);
    for(register uint8_t i = 1; i < 4; i++) {
        gpio_set_irq_enabled(col[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }

    gpio_put(LED_CON, true);
    
    while(true) {
        KSP_DATA flight_data = uart_data_decoder(uart_str_in);
        led_update(flight_data);

        switch (verb) {
        case 0:
            switch (noun) {
            // Disconnect from server
            case 1:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);

                cancel_repeating_timer(&timer_uart_send);
                time_since_last_connection = 6;
                flight_data = uart_data_decoder(UART_EMPTY_STRING);
                led_update(flight_data);

                memset(uart_str_in, 0, UART_STR_IN_BUF_SIZE);
                goto handshake;

                break;
            // Restart DSKY
            case 2:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                sleep_ms(500);
                software_reset();
                break;
                
            default:
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                break;
            } 
            break;
        
        case 1:
            if(noun == 2) {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_float_number(0, 1, (float)flight_data.num1 / 1000);
                    display_float_number(1, 0, (float)flight_data.num2 / 1000);
                    display_float_number(1, 1, (float)flight_data.num3 / 1000);
                }
            } else {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_number(1, 1, flight_data.num3);
                }
            }
            break;

        case 2:
            if(noun == 6) {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_float_number(0, 1, (float)flight_data.num1 / 1000);
                    display_float_number(1, 0, (float)flight_data.num2 / 1000);
                    display_float_number(1, 1, (float)flight_data.num3 / 1000);
                }
            } else if((noun >= 7) && (noun <= 9)) {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_float_number(1, 1, (float)flight_data.num3 / 1000);
                }
            } else {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_number(1, 1, flight_data.num3);
                }
            }
            break;

        case 3:
            if((noun == 2) || (noun == 3)) {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_float_number(1, 1, (float)flight_data.num3 / 1000);
                }
            } else {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_number(1, 1, flight_data.num3);
                }
            }
            break;

        case 4:
            if(noun == 3) {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);

                    float dv = (float)flight_data.num1 / 1000;
                    float rdv = (float)flight_data.num2 / 1000;

                    (dv < 10000) ? display_float_number(0, 1, dv) : display_number(0, 1, (int64_t)dv);
                    (rdv < 10000) ? display_float_number(1, 0, rdv) : display_number(1, 0, (int64_t)rdv);

                    display_number(1, 1, flight_data.num3);
                }
            } else if(noun == 4) {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_float_number(1, 1, (float)flight_data.num3 / 1000);
                }
            } else {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_number(1, 1, flight_data.num3);
                }
            }

            break;
        
        case 5:
            if((noun == 1) || (noun == 2)) {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_number(0, 1, flight_data.num1);
                    display_float_number(1, 0, (float)flight_data.num2 / 1000);
                    display_float_number(1, 1, (float)flight_data.num3 / 1000);
                }
            } else {
                clear_display(0, 1);
                clear_display(1, 0);
                clear_display(1, 1);
                while (keypad_status != KEY_STAT_PRG_CHANGE) {
                    flight_data = uart_data_decoder(uart_str_in);
                    led_update(flight_data);
                    display_number(0, 1, flight_data.num1);
                    display_number(1, 0, flight_data.num2);
                    display_number(1, 1, flight_data.num3);
                }
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
                        led_update(flight_data);
                        display_number(0, 1, flight_data.num1);
                        display_number(1, 0, flight_data.num2);
                        display_number(1, 1, flight_data.num3);
                    }
                    break;
                
                // Display float
                case 3:
                    display_float_number(0, 1, 643.52);
                    display_float_number(1, 0, 0.0);
                    display_float_number(1, 1, -0.64352);
                
                // Testing number input
                case 4:
                    clear_display(0, 1);
                    clear_display(1, 0);
                    clear_display(1, 1);

                    float insnum = request_number(1, 0);
                    
                    while(keypad_status != KEY_STAT_PRG_CHANGE) {
                        clear_display(0, 1);
                        clear_display(1, 0);
                        display_float_number(1, 1, insnum);
                    }

                    break;

                case 5:
                    clear_display(0, 1);
                    clear_display(1, 0);
                    clear_display(1, 1);

                    sleep_ms(1000);

                    insnms[0] = request_number(0, 1);
                    insnms[1] = request_number(1, 0);
                    insnms[2] = request_number(1, 1);

                    noun = 6;
                    clear_display(0, 0);
                    add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program

                    break;

                case 6:
                    while(keypad_status != KEY_STAT_PRG_CHANGE) {
                        flight_data = uart_data_decoder(uart_str_in);
                        led_update(flight_data);
                        display_float_number(0, 1, insnms[0]);
                        display_float_number(1, 0, insnms[1]);
                        display_float_number(1, 1, insnms[2]);
                    }

                    break;

                default:
                    break;
                }
        default:
            led_update(flight_data);
            clear_display(0, 1);
            clear_display(1, 0);
            clear_display(1, 1);
            break;
        }
    }
}

bool timer_uart_send_handler(__unused struct repeating_timer *t) {
    char buffer[50];
    // printf("Verb: %u Noun: %u\n", verb, noun);
    sprintf(buffer, "V:%u;N:%u\n", verb, noun);
    uart_puts(UART_ID, buffer);

    return true;
}

void software_reset() {
    watchdog_enable(1, 1);
    while(1);
}

#pragma clang diagnostic pop