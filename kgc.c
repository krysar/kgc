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
#include "errors.h"

#define ASC_INS_3_NUMS(prev, next) {\
    if((prev_verb == 13) && (prev_noun == prev)) { \
        SAVE_PREV_VERB_NOUN \
\
        clear_display(0, 1); \
        clear_display(1, 0); \
        clear_display(1, 1); \
\
        sleep_ms(500); \
\
        insnms[0] = request_number(0, 1); \
        insnms[1] = request_number(1, 0); \
        insnms[2] = request_number(1, 1); \
\
        sleep_ms(100); \
\
        send_data[0] = insnms[0]; \
        send_data[1] = insnms[1]; \
        send_data[2] = insnms[2]; \
\
        sleep_ms(1500); \
\
        noun = next; \
        send_data[0] = -2; \
        send_data[1] = -2; \
        send_data[2] = -2; \
        clear_display(0, 0); \
        add_alarm_in_ms(800, display_program, NULL, false); /* 800 ms blank, then display program */ \
        sleep_ms(900); \
    } else { \
        while(keypad_status != KEY_STAT_PRG_CHANGE) { \
            flight_data = uart_data_decoder(uart_str_in); \
            led_update(flight_data); \
            clear_display(0, 1); \
            display_err_code(ERR_ASC_UNINITIALIZED); \
        } \
    } \
}

#define ASC_INS_2_NUMS(prev, next) {\
    if((prev_verb == 13) && (prev_noun == prev)) { \
        SAVE_PREV_VERB_NOUN \
\
        clear_display(0, 1); \
        clear_display(1, 0); \
        clear_display(1, 1); \
\
        sleep_ms(500); \
\
        insnms[0] = request_number(0, 1); \
        insnms[1] = request_number(1, 0); \
\
        sleep_ms(100); \
\
        send_data[0] = insnms[0]; \
        send_data[1] = insnms[1]; \
        send_data[2] = 0; \
\
        sleep_ms(1500); \
\
        noun = next; \
        send_data[0] = -2; \
        send_data[1] = -2; \
        send_data[2] = -2; \
        clear_display(0, 0); \
        add_alarm_in_ms(800, display_program, NULL, false); /* 800 ms blank, then display program */ \
        sleep_ms(900); \
    } else { \
        while(keypad_status != KEY_STAT_PRG_CHANGE) { \
            flight_data = uart_data_decoder(uart_str_in); \
            led_update(flight_data); \
            clear_display(0, 1); \
            display_err_code(ERR_ASC_UNINITIALIZED); \
        } \
    } \
}

#define ASC_DISP_2_NUMS(prev, next) {\
    if((prev_verb == 13) && (prev_noun == prev)) { \
        SAVE_PREV_VERB_NOUN \
        led_stat[LED_STAT_DISP1] = true;\
        keypad_status = KEY_STAT_NO_CHANGE;\
        while(keypad_status != KEY_STAT_PROCEED) { \
            flight_data = uart_data_decoder(uart_str_in); \
            led_update(flight_data); \
            display_number(0, 1, flight_data.num1); \
            display_number(1, 0, flight_data.num2); \
            clear_display(1, 1); \
        } \
\
        led_stat[LED_STAT_DISP1] = false;\
        noun = next; \
        clear_display(0, 0); \
        add_alarm_in_ms(800, display_program, NULL, false); /* 800 ms blank, then display program */ \
        sleep_ms(900); \
    } else { \
        while(keypad_status != KEY_STAT_PRG_CHANGE) { \
            flight_data = uart_data_decoder(uart_str_in); \
            led_update(flight_data); \
            clear_display(0, 1); \
            display_err_code(ERR_ASC_UNINITIALIZED); \
        } \
    } \
}

#define ASC_DISP_3_NUMS(prev, next) {\
    if((prev_verb == 13) && (prev_noun == prev)) { \
        SAVE_PREV_VERB_NOUN \
        led_stat[LED_STAT_DISP1] = true;\
        keypad_status = KEY_STAT_NO_CHANGE;\
        while(keypad_status != KEY_STAT_PROCEED) { \
            flight_data = uart_data_decoder(uart_str_in); \
            led_update(flight_data); \
            display_number(0, 1, flight_data.num1); \
            display_number(1, 0, flight_data.num2); \
            display_number(1, 1, flight_data.num3); \
        } \
\
        led_stat[LED_STAT_DISP1] = false;\
        noun = next; \
        clear_display(0, 0); \
        add_alarm_in_ms(800, display_program, NULL, false); /* 800 ms blank, then display program */ \
        sleep_ms(900); \
    } else { \
        while(keypad_status != KEY_STAT_PRG_CHANGE) { \
            flight_data = uart_data_decoder(uart_str_in); \
            led_update(flight_data); \
            clear_display(0, 1); \
            display_err_code(ERR_ASC_UNINITIALIZED); \
        } \
    } \
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

float insnms[3] = {0};
uint8_t asc_profile = 4;

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
    prev_verb = 0, prev_noun = 0;
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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
                SAVE_PREV_VERB_NOUN
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

        // Basic ship control
        case 12:
            sleep_ms(1500);
            verb = prev_verb;
            noun = prev_noun;
            clear_display(0, 0);
            add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program

            break;

        // Ascent autopilot
        case 13:
            switch(noun) {
                // Init ascent autopilot
                case 0:
                    sleep_ms(1000);
                    SAVE_PREV_VERB_NOUN
                    send_data[0] = -2; 
                    send_data[1] = -1000; 
                    send_data[2] = -2; 
                    noun = 1;
                    clear_display(0, 0);
                    add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program
                    sleep_ms(900);
                    break;

                // Insert orbit altitude, inclination, ascent profile
                case 1:
                    if((prev_verb == 13) && (prev_noun == 0)) {
                        SAVE_PREV_VERB_NOUN

                        clear_display(0, 1);
                        clear_display(1, 0);
                        clear_display(1, 1);

                        sleep_ms(500);

                        insnms[0] = request_number(0, 1);
                        insnms[1] = request_number(1, 0);
                        insnms[2] = request_number(1, 1);

                        sleep_ms(100);

                        send_data[0] = insnms[0];
                        send_data[1] = insnms[1];
                        send_data[2] = insnms[2];
                        asc_profile = (uint8_t)insnms[2];

                        sleep_ms(1500);

                        noun = 2;
                        send_data[0] = -2; 
                        send_data[1] = -2; 
                        send_data[2] = -2; 
                        clear_display(0, 0);
                        add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program
                        sleep_ms(900);
                    } else {
                        while(keypad_status != KEY_STAT_PRG_CHANGE) {
                            flight_data = uart_data_decoder(uart_str_in);
                            led_update(flight_data);
                            clear_display(0, 1);
                            display_err_code(ERR_ASC_UNINITIALIZED);
                        }
                    }
                    break;

                // Display orbit altitude, inclination, ascent profile for confirmation
                case 2:
                    ASC_DISP_3_NUMS(1, 3)
                    break;

                // Insert acceleration limit, throttle limit
                case 3:
                    ASC_INS_2_NUMS(2, 4)
                    break;

                // Display acceleration limit, throttle limit for confirmation
                case 4:
                    ASC_DISP_2_NUMS(3, 5)
                    break;

                // Insert force roll: climb, turn
                case 5:
                    ASC_INS_2_NUMS(4, 6)
                    break;

                // Display force roll: climb, turn for confirmation
                case 6:
                    ASC_DISP_2_NUMS(5, 7)
                    break;

                // Insert autostage: pre-delay, post-delay, clamp autostage thurst
                case 7:
                    ASC_INS_3_NUMS(6, 8)
                    break;

                // Display autostage: pre-delay, post-delay, clamp autostage thurst for confirmation
                case 8:
                    ASC_DISP_3_NUMS(7, 9)
                    break;

                // Insert min throttle, stop stage for autostage, other options
                case 9:
                    ASC_INS_3_NUMS(8, 10)
                    break;

                // Display min throttle, stop stage for auto stage, other options for confirmation
                case 10:
                    if((prev_verb == 13) && (prev_noun == 9)) {
                        SAVE_PREV_VERB_NOUN
                        led_stat[LED_STAT_DISP1] = true;
                        while(keypad_status != KEY_STAT_PROCEED){
                            flight_data = uart_data_decoder(uart_str_in);
                            led_update(flight_data);
                            display_number(0, 1, flight_data.num1);
                            display_number(1, 0, flight_data.num2);
                            display_number(1, 1, flight_data.num3);
                        }

                        switch(asc_profile) {
                            case 1:
                                noun = 11;
                                break;
                            case 2:
                                noun = 15;
                                break;
                            case 3:
                                noun = 19;
                                break;
                            default:
                                while(keypad_status != KEY_STAT_PRG_CHANGE) {
                                    flight_data = uart_data_decoder(uart_str_in);
                                    led_update(flight_data);
                                    clear_display(0, 1);
                                    display_err_code(ERR_ASC_BAD_ASC_PROFILE);
                                }
                        }

                        led_stat[LED_STAT_DISP1] = false;
                        clear_display(0, 0);
                        add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program
                        sleep_ms(900);
                    } else {
                        while(keypad_status != KEY_STAT_PRG_CHANGE) {
                            flight_data = uart_data_decoder(uart_str_in);
                            led_update(flight_data);
                            clear_display(0, 1);
                            display_err_code(ERR_ASC_UNINITIALIZED);
                        }
                    }
                    break;

                // Classic ascent profile
                // Insert turn start altitude, turn start velocity, turn end altitude
                case 11:
                    ASC_INS_3_NUMS(10, 12)
                    break;

                // Display turn start altitude, turn start velocity, turn end altitude for confirmation
                case 12:
                    ASC_DISP_3_NUMS(11, 13)
                    break;

                // Insert final flight path angle, turn shape
                case 13:
                    ASC_INS_2_NUMS(12, 14)
                    break;

                // Display final flight path angle, turn shape for confirmation
                case 14:
                    ASC_DISP_2_NUMS(13, 25)
                    break;
                
                // Stock style gravity turn
                // Insert turn start altitude, turn start velocity, turn start pitch
                case 15:
                    ASC_INS_3_NUMS(10, 16)
                    break;

                // Display turn start altitude, turn start velocity, turn start pitch
                case 16:
                    ASC_DISP_3_NUMS(15, 17)
                    break;

                // Insert intermediate altitude, hold AP time
                case 17:
                    ASC_INS_2_NUMS(16, 18)
                    break;

                // Display intermediate altitude, hold AP time for confirmation
                case 18:
                    ASC_DISP_2_NUMS(17, 25)
                    break;

                // PVG ascent profile
                // Insert target apoapsis, attach altitude, booster pitch start
                case 19:
                    ASC_INS_3_NUMS(10, 20)
                    break;

                // Display target apoapsis, attach altitude, booster pitch start for confirmation
                case 20:
                    ASC_DISP_3_NUMS(19, 21)
                    break;

                // Insert booster pitch rate, PVG after stage
                case 21:
                    ASC_INS_2_NUMS(20, 22)
                    break;

                // Display booster pitch rate, PVG after stage for confirmation
                case 22:
                    ASC_DISP_2_NUMS(21, 23)
                    break;

                // Insert Q trigger, fixed coast length
                case 23:
                    ASC_INS_2_NUMS(22, 24)
                    break;
                
                // Display Q trigger, fixed coast length for confirmation
                case 24:
                    ASC_DISP_2_NUMS(23, 25)
                    break;

                // Enable ascent autopilot
                case 25:
                    if((prev_verb == 13) && ((prev_noun == 14) || (prev_noun == 18) || (prev_noun == 24))) {
                        led_stat[LED_STAT_DISP1] = true;
                        clear_display(0, 1);
                        clear_display(1, 0);
                        clear_display(1, 1);

                        send_data[0] = -2;
                        send_data[1] = -2;
                        send_data[2] = -2;

                        while(keypad_status != KEY_STAT_PROCEED) {
                            flight_data = uart_data_decoder(uart_str_in);
                            led_update(flight_data);
                        }

                        send_data[0] = 1;
                        sleep_ms(1500);

                        verb = 3;
                        noun = 1;

                        led_stat[LED_STAT_DISP1] = false;
                        clear_display(0, 0);
                        add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program
                        sleep_ms(900);
                    } else {
                        while(keypad_status != KEY_STAT_PRG_CHANGE) {
                            flight_data = uart_data_decoder(uart_str_in);
                            led_update(flight_data);
                            clear_display(0, 1);
                            display_err_code(ERR_ASC_UNINITIALIZED);
                        }
                    }
                    break;

                // Disable ascent autopilot
                case 26:
                    led_stat[LED_STAT_DISP1] = true;
                    clear_display(0, 1);
                    clear_display(1, 0);
                    clear_display(1, 1);

                    send_data[0] = -2;
                    send_data[1] = -2;
                    send_data[2] = -2;

                    while(keypad_status != KEY_STAT_PROCEED) {
                        flight_data = uart_data_decoder(uart_str_in);
                        led_update(flight_data);
                    }

                    send_data[0] = 1;
                    sleep_ms(1500);

                    verb = prev_noun;
                    noun = prev_verb;

                    led_stat[LED_STAT_DISP1] = false;
                    clear_display(0, 0);
                    add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program
                    sleep_ms(900);

                default:
                    while(keypad_status != KEY_STAT_PRG_CHANGE) {
                        flight_data = uart_data_decoder(uart_str_in);
                        led_update(flight_data);
                        display_err_code(ERR_ASC_NON_EXIST_NOUN);
                    }
            }
            break;

        // Testing verb
        case 99:
            switch (noun) {
                // Display 3 static numbers
                case 1:
                    SAVE_PREV_VERB_NOUN
                    while(keypad_status != KEY_STAT_PRG_CHANGE) {
                        display_number(0, 1, 1234);
                        display_number(1, 0, 5678);
                        display_number(1, 1, 9012);
                    }
                    break;
                
                // Display incrementing number from server
                case 2:
                    SAVE_PREV_VERB_NOUN
                    clear_display(0, 1);
                    clear_display(1, 0);
                    clear_display(1, 1);
                    while(keypad_status != KEY_STAT_PRG_CHANGE) {
                        flight_data = uart_data_decoder(uart_str_in);
                        led_update(flight_data);
                        display_number(0, 1, flight_data.num1);
                        display_number(1, 0, flight_data.num2);
                        display_number(1, 1, flight_data.num3);
                    }
                    break;
                
                // Display float
                case 3:
                    SAVE_PREV_VERB_NOUN
                    while(keypad_status != KEY_STAT_PRG_CHANGE) {
                        display_float_number(0, 1, 643.52);
                        display_float_number(1, 0, 0.0);
                        display_float_number(1, 1, -0.64352);
                    }
                    break;
                
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
                    SAVE_PREV_VERB_NOUN

                    break;

                case 5:
                    clear_display(0, 1);
                    clear_display(1, 0);
                    clear_display(1, 1);

                    sleep_ms(500);

                    insnms[0] = request_number(0, 1);
                    insnms[1] = request_number(1, 0);
                    insnms[2] = request_number(1, 1);

                    noun = 6;
                    clear_display(0, 0);
                    add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program

                    break;

                case 6:
                    led_stat[LED_STAT_DISP1] = true;
                    while(keypad_status != KEY_STAT_PROCEED) {
                        flight_data = uart_data_decoder(uart_str_in);
                        led_update(flight_data);
                        display_float_number(0, 1, insnms[0]);
                        display_float_number(1, 0, insnms[1]);
                        display_float_number(1, 1, insnms[2]);
                    }
                    
                    verb = prev_verb;
                    noun = prev_noun;
                    led_stat[LED_STAT_DISP1] = false;
                    clear_display(0, 0);
                    add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program
                    
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
    char buffer[128];
    // printf("Verb: %u Noun: %u\n", verb, noun);
    sprintf(buffer, "V:%u;N:%u;D1:%f;D2:%f;D3:%f\n", verb, noun, send_data[0], send_data[1], send_data[2]);
    uart_puts(UART_ID, buffer);

    return true;
}

void software_reset() {
    watchdog_enable(1, 1);
    while(1);
}

#pragma clang diagnostic pop