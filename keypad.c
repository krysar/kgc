#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "keypad.h"
#include "max7219.h"

uint8_t keypad_status = 0, section = 0, command = 0, change_register;
uint8_t section_choice[] = {254, 254}, command_choice[] = {254, 254};
bool debounce = false;

void keypad_init() {
    for(register uint8_t i = 0; i < 4; i++) {
        gpio_init(col[i]);
        gpio_init(row[i]);

        gpio_set_dir(col[i], GPIO_IN);
        gpio_set_dir(row[i], GPIO_OUT);

        gpio_put(row[i], 1);

        col[i] = col[i];
        row[i] = row[i];
    }
}

int64_t display_program(alarm_id_t id, void *user_data) {
    display_two_digit_number(0, 0, 0, section);
    display_two_digit_number(0, 0, 1, command);
    keypad_status = 0;
    return 0;
}

int64_t debounce_unset(alarm_id_t id, void *user_data) {
    debounce = false;
    return 0;
}

uint8_t key_evaluate(uint8_t pressed_key) {
    if((pressed_key == 10) && (section_choice[0] == 254) && (section_choice[1] == 254)) {
        spi_send_data(0, REG_DIGIT0, code_table[24]);
        spi_send_data(0, REG_DIGIT1, code_table[24]);
        section_choice[0] = 255;
        return 0;
    } else if((pressed_key == 11) && (command_choice[0] == 254) && (section_choice[1] == 254)) {
        spi_send_data(0, REG_DIGIT2, code_table[24]);
        spi_send_data(0, REG_DIGIT3, code_table[24]);
        command_choice[0] = 255;
        return 0;
    } else if(section_choice[0] == 255) {
        if(pressed_key >= 10)
            return 0;
        section_choice[0] = pressed_key;
        section_choice[1] = 255;
        spi_send_data(0, REG_DIGIT0, code_table[pressed_key]);
        return 0;
    } else if(command_choice[0] == 255) {
        if(pressed_key >= 10)
            return 0;
        command_choice[0] = pressed_key;
        command_choice[1] = 255;
        spi_send_data(0, REG_DIGIT2, code_table[pressed_key]);
        return 0;
    } else if(section_choice[1] == 255) {
        if(pressed_key >= 10)
            return 0;
        section_choice[1] = pressed_key;
        spi_send_data(0, REG_DIGIT1, code_table[pressed_key]);
        return 0;
    } else if(command_choice[1] == 255) {
        if(pressed_key >= 10)
            return 0;
        command_choice[1] = pressed_key;
        spi_send_data(0, REG_DIGIT3, code_table[pressed_key]);
        return 0;
    } else if((pressed_key == 15) && (section_choice[1] < 254) && (command_choice[1] >= 254)) {
        spi_send_data(0, REG_DIGIT2, code_table[24]);
        spi_send_data(0, REG_DIGIT3, code_table[24]);
        command_choice[0] = 255;
        return 0;
    } else if((pressed_key == 15) && (command_choice[1] < 254)) {
        if(section_choice[1] < 254) {
            section = 0;
            section += section_choice[0] * 10;
            section += section_choice[1];
        }

        command = 0;
        command += command_choice[0] * 10;
        command += command_choice[1];

        section_choice[0] = 254;
        section_choice[1] = 254;
        command_choice[0] = 254;
        command_choice[1] = 254;

        spi_send_data(0, REG_DIGIT0, CODE_BLANK);
        spi_send_data(0, REG_DIGIT1, CODE_BLANK);
        spi_send_data(0, REG_DIGIT2, CODE_BLANK);
        spi_send_data(0, REG_DIGIT3, CODE_BLANK);

        add_alarm_in_ms(800, display_program, NULL, false);

        return 1; // program change
    } else if(keypad_status == 2) {

    }
    else {
        return 0;
    }
}

void keypad_irq_handler(uint gpio, uint32_t events) {
    if(debounce == true)
        return;
    else {
        for (register uint8_t i = 0; i < 4; i++)
            gpio_put(row[i], 0);

        for (register uint8_t i = 0; i < 4; i++) {
            gpio_put(row[i], 1);
            for (register uint8_t j = 0; j < 4; j++) {
                if (gpio_get(col[j])) {
                    while (gpio_get(col[j])) {

                    }
                    keypad_status = key_evaluate(keymap[i][j]);
                    debounce = true;
                    add_alarm_in_ms(50, debounce_unset, NULL, false);
                }
            }
            gpio_put(row[i], 0);
        }

        for (register uint8_t i = 0; i < 4; i++)
            gpio_put(row[i], 1);
    }
}