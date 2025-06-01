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
uint8_t hrs, min, sec;
int8_t milisec100;

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

    //Welcome message
    spi_send_data(0, REG_DIGIT0, code_table[17]); // K
    spi_send_data(0, REG_DIGIT1, code_table[6]);  // G
    spi_send_data(0, REG_DIGIT2, code_table[12]); // c
    spi_send_data(0, REG_DIGIT4, code_table[24]); // v
    spi_send_data(0, REG_DIGIT5, code_table[14]); // E
    spi_send_data(0, REG_DIGIT6, code_table[22]); // r
    spi_send_data(1, REG_DIGIT0, code_table[0]);  // 0
    spi_send_data(1, REG_DIGIT2, code_table[0]);  // 0
    spi_send_data(1, REG_DIGIT3, code_table[2]);  // 2

    sleep_ms(2000);

    display_number(0, 0, 0);


    gpio_set_irq_enabled_with_callback(col[0], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &keypad_irq_handler);
    for(register uint8_t i = 1; i < 4; i++) {
        gpio_set_irq_enabled(col[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    }

    struct repeating_timer timer;
    add_repeating_timer_ms(100, timer_handler, NULL, &timer);

    while(true) {
        printf("Verb: %u Noun: %u\n", verb, noun);
        sleep_ms(1000);

        switch (verb)
        {
        case 00:
            clear_display(0, 1);
            clear_display(1, 0);
            clear_display(1, 1);
            break;
        
        case 01:
            display_number(0, 1, 1234);
            display_number(1, 0, 5678);
            display_number(1, 1, 1479);
            break;

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

bool timer_handler(struct repeating_timer *t) {
    return true;
}

#pragma clang diagnostic pop