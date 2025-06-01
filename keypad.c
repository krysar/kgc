#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "keypad.h"
#include "max7219.h"

uint8_t keypad_status = 0, verb = 0, noun = 0, change_register;
uint8_t verb_choice[] = {254, 254}, noun_choice[] = {254, 254};
uint16_t insert[3] = {};
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

// Function is called by alarm to display verb and noun numb. after 800 ms blank
int64_t display_program(alarm_id_t id, void *user_data) {
    display_two_digit_number(0, 0, 0, verb);
    display_two_digit_number(0, 0, 1, noun);
    keypad_status = 0;
    return 0;
}

uint16_t revert_number(uint16_t number) {
    uint16_t rev = 0, rem;

    while (number != 0) {
        rem = number % 10;
        rev = rev * 10 + rem;
        number /= 10;
    }

    return rev;
}

int64_t debounce_unset(alarm_id_t id, void *user_data) {
    debounce = false;
    return 0;
}

uint8_t key_evaluate(uint8_t pressed_key) {
    if(keypad_status == 2) {
        if(pressed_key == KEY_ENTER) {
            insert[0] = revert_number(insert[0]);
            spi_send_data(0, REG_DIGIT4, CODE_BLANK);
            spi_send_data(0, REG_DIGIT5, CODE_BLANK);
            spi_send_data(0, REG_DIGIT6, CODE_BLANK);
            spi_send_data(0, REG_DIGIT7, CODE_BLANK);
            return KEY_STAT_NUM_INSERTED; // number inserted
        } else if(pressed_key <= 9) {
            if (insert[0] == 0) {
                insert[0] = pressed_key;
                spi_send_data(0, REG_DIGIT4, code_table[pressed_key]);
            } else if ((insert[0] > 0) && (insert[0] < 10)) {
                insert[0] += pressed_key * 10;
                spi_send_data(0, REG_DIGIT5, code_table[pressed_key]);
            } else if ((insert[0] >= 10) && (insert[0] < 100)) {
                insert[0] += pressed_key * 100;
                spi_send_data(0, REG_DIGIT6, code_table[pressed_key]);
            } else if((insert[0] >= 100) && (insert[0] < 1000)) {
                insert[0] += pressed_key * 1000;
                spi_send_data(0, REG_DIGIT7, code_table[pressed_key]);
            } else {
                //todo: OTR LED blinking
            }
        }

        return KEY_STAT_NUM_INSERT;
    }
    // KEY_VERB is pressed without any previous verb operation
    // Preparation for insert 1st verb digit
    if ((pressed_key == KEY_VERB) && (verb_choice[0] == 254) && (verb_choice[1] == 254)) {
        spi_send_data(0, REG_DIGIT0, code_table[25]);
        spi_send_data(0, REG_DIGIT1, code_table[25]);
        verb_choice[0] = 255;
        return KEY_STAT_NO_CHANGE;
        // KEY_NOUN is pressed without any previous noun operation
        // Preparation for insert 1st noun digit
    } else if ((pressed_key == KEY_NOUN) && (noun_choice[0] == 254) && (verb_choice[1] == 254)) {
        spi_send_data(0, REG_DIGIT2, code_table[25]);
        spi_send_data(0, REG_DIGIT3, code_table[25]);
        noun_choice[0] = 255;
        return KEY_STAT_NO_CHANGE;
        // Inserting 1st verb digit, preparation for insert 2nd
    } else if (verb_choice[0] == 255) {
        if (pressed_key >= 10) // We're accepting 0 - 10 keys only
            return KEY_STAT_NO_CHANGE;
        verb_choice[0] = pressed_key;
        verb_choice[1] = 255;
        spi_send_data(0, REG_DIGIT0, code_table[pressed_key]);
        return KEY_STAT_NO_CHANGE;
        // Inserting 1st noun digit, preparation for insert 2nd
    } else if(noun_choice[0] == 255) {
        if(pressed_key >= 10)
            return KEY_STAT_NO_CHANGE;
        noun_choice[0] = pressed_key;
        noun_choice[1] = 255;
        spi_send_data(0, REG_DIGIT2, code_table[pressed_key]);
        return KEY_STAT_NO_CHANGE;
        // Inserting 2nd verb digit
    } else if(verb_choice[1] == 255) {
        if(pressed_key >= 10)
            return KEY_STAT_NO_CHANGE;
        verb_choice[1] = pressed_key;
        spi_send_data(0, REG_DIGIT1, code_table[pressed_key]);
        return KEY_STAT_NO_CHANGE;
        // Inserting 2nd noun digit
    } else if (noun_choice[1] == 255) {
        if (pressed_key >= 10)
            return KEY_STAT_NO_CHANGE;
        noun_choice[1] = pressed_key;
        spi_send_data(0, REG_DIGIT3, code_table[pressed_key]);
        return KEY_STAT_NO_CHANGE;
        // KEY_ENTER is pressed, verb only is inserted
        // It's necessary to insert also noun - preparation for insert 1st noun digit
    } else if ((pressed_key == KEY_ENTER) && (verb_choice[1] < 254) && (noun_choice[1] >= 254)) {
        spi_send_data(0, REG_DIGIT2, code_table[25]);
        spi_send_data(0, REG_DIGIT3, code_table[25]);
        noun_choice[0] = 255;
        return KEY_STAT_NO_CHANGE;
        // KEY_ENTER is pressed, at least noun is inserted
        // Test if verb is inserted -> change program variable -> display new program -> return 1 (program change)
    } else if ((pressed_key == KEY_ENTER) && (noun_choice[1] < 254)) {
        if (verb_choice[1] < 254) {
            verb = 0;
            verb += verb_choice[0] * 10;
            verb += verb_choice[1];
        }

        noun = 0;
        noun += noun_choice[0] * 10;
        noun += noun_choice[1];

        verb_choice[0] = 254;
        verb_choice[1] = 254;
        noun_choice[0] = 254;
        noun_choice[1] = 254;

        spi_send_data(0, REG_DIGIT0, CODE_BLANK);
        spi_send_data(0, REG_DIGIT1, CODE_BLANK);
        spi_send_data(0, REG_DIGIT2, CODE_BLANK);
        spi_send_data(0, REG_DIGIT3, CODE_BLANK);

        add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program

        return KEY_STAT_PRG_CHANGE; // program change
    //Insert 1 reg.
    } else {
        return KEY_STAT_NO_CHANGE;
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