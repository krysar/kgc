#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "keypad.h"
#include "max7219.h"

uint8_t keypad_status = 0, section = 0, command = 0, change_register;
uint8_t section_choice[] = {254, 254}, command_choice[] = {254, 254};
uint8_t program[4] = {}, prev_program[4] = {};
bool debounce = false;

// Init function for keypad
void keypad_init() {
    for(register uint8_t i = 0; i < 4; i++) {
        gpio_init(col[i]);
        gpio_init(row[i]);

        gpio_set_dir(col[i], GPIO_IN); // Column pins are input
        gpio_set_dir(row[i], GPIO_OUT); // Row pins are output

        gpio_put(row[i], 1); // We're going to set all row pins to 1 to be able to detect a keystroke
    }
}

// Function is called by alarm to display sec. and comm. numb. after 800 ms blank
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
    // KEY_SECTION is pressed without any previous section operation
    // Preparation for insert 1st section digit
    if ((pressed_key == KEY_SECTION) && (section_choice[0] == 254) && (section_choice[1] == 254)) {
        spi_send_data(0, REG_DIGIT0, code_table[24]);
        spi_send_data(0, REG_DIGIT1, code_table[24]);
        section_choice[0] = 255;
        return 0;
        // KEY_COMMAND is pressed without any previous command operation
        // Preparation for insert 1st command digit
    } else if ((pressed_key == KEY_COMMAND) && (command_choice[0] == 254) && (section_choice[1] == 254)) {
        spi_send_data(0, REG_DIGIT2, code_table[24]);
        spi_send_data(0, REG_DIGIT3, code_table[24]);
        command_choice[0] = 255;
        return 0;
        // Inserting 1st section digit, preparation for insert 2nd
    } else if (section_choice[0] == 255) {
        if (pressed_key >= 10) // We're accepting 0 - 10 keys only
            return 0;
        section_choice[0] = pressed_key;
        section_choice[1] = 255;
        spi_send_data(0, REG_DIGIT0, code_table[pressed_key]);
        return 0;
        // Inserting 1st command digit, preparation for insert 2nd
    } else if(command_choice[0] == 255) {
        if(pressed_key >= 10)
            return 0;
        command_choice[0] = pressed_key;
        command_choice[1] = 255;
        spi_send_data(0, REG_DIGIT2, code_table[pressed_key]);
        return 0;
        // Inserting 2nd section digit
    } else if(section_choice[1] == 255) {
        if(pressed_key >= 10)
            return 0;
        section_choice[1] = pressed_key;
        spi_send_data(0, REG_DIGIT1, code_table[pressed_key]);
        return 0;
        // Inserting 2nd command digit
    } else if (command_choice[1] == 255) {
        if (pressed_key >= 10)
            return 0;
        command_choice[1] = pressed_key;
        spi_send_data(0, REG_DIGIT3, code_table[pressed_key]);
        return 0;
        // KEY_ENTER is pressed, section only is inserted
        // It's necessary to insert also command - preparation for insert 1st command digit
    } else if ((pressed_key == KEY_ENTER) && (section_choice[1] < 254) && (command_choice[1] >= 254)) {
        spi_send_data(0, REG_DIGIT2, code_table[24]);
        spi_send_data(0, REG_DIGIT3, code_table[24]);
        command_choice[0] = 255;
        return 0;
        // KEY_ENTER is pressed, at least command is inserted
        // Test if section is inserted -> change program variable -> display new program -> return 1 (program change)
    } else if ((pressed_key == KEY_ENTER) && (command_choice[1] < 254)) {
        if (section_choice[1] < 254) {
            section = 0;
            section += section_choice[0] * 10;
            section += section_choice[1];
        }

        command = 0;
        command += command_choice[0] * 10;
        command += command_choice[1];

        if (section < 10) {
            program[1] = section;
        }

        section_choice[0] = 254;
        section_choice[1] = 254;
        command_choice[0] = 254;
        command_choice[1] = 254;

        spi_send_data(0, REG_DIGIT0, CODE_BLANK);
        spi_send_data(0, REG_DIGIT1, CODE_BLANK);
        spi_send_data(0, REG_DIGIT2, CODE_BLANK);
        spi_send_data(0, REG_DIGIT3, CODE_BLANK);

        add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program

        return 1; // program change
    } else if(keypad_status == 2) {

    }
    else {
        return 0;
    }
}

void keypad_irq_handler(uint gpio, uint32_t events) {
    if (debounce == true)
        return; // Still debouncing
    else {
        // We're going to set all row pins to 0
        for (register uint8_t i = 0; i < 4; i++)
            gpio_put(row[i], 0);

        // Then we gonna test row by row to find pressed key
        for (register uint8_t i = 0; i < 4; i++) {
            gpio_put(row[i], 1);
            for (register uint8_t j = 0; j < 4; j++) {
                if (gpio_get(col[j])) {
                    while (gpio_get(col[j])) {
                        // Do nothing while key is pressed
                    }
                    keypad_status = key_evaluate(keymap[i][j]); // Then evaluate key meaning
                    debounce = true;
                    add_alarm_in_ms(50, debounce_unset, NULL, false);
                }
            }
            gpio_put(row[i], 0);
        }

        for (register uint8_t i = 0; i < 4; i++)
            gpio_put(row[i], 1); // Set all row pins to 1 to be able to detect a keystroke
    }
}