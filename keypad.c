/*
 * Library for user inputs from keypad and program control
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "keypad.h"
#include "max7219.h"
#include "led.h"

uint8_t keypad_status = 0, verb = 0, prev_verb = 0, noun = 0, prev_noun = 0, request_num = NO_NUM_TO_READ, change_register;
uint8_t verb_choice[] = {254, 254}, noun_choice[] = {254, 254};
uint8_t dots_count = 0;
char inserted_num_buf[INSERTED_NUM_BUF_SIZE] = "";
float inserted_num[3] = {0};
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
    keypad_status = KEY_STAT_NO_CHANGE;
    return 0;
}

int64_t debounce_unset(alarm_id_t id, void *user_data) {
    debounce = false;
    return 0;
}

uint8_t key_evaluate(uint8_t pressed_key) {
    if(pressed_key == KEY_PROCEED) {
        printf("Proceed\n");
        return KEY_STAT_PROCEED;
    }
    // KEY_VERB is pressed without any previous verb operation
    // Preparation for insert 1st verb digit
    if ((pressed_key == KEY_VERB) && (verb_choice[0] == 254) && (verb_choice[1] == 254)) {
        spi_send_data(0, REG_DIGIT0, code_table[25]);
        spi_send_data(0, REG_DIGIT1, code_table[25] + CODE_DP);
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
        if (pressed_key >= 10) // We're accepting 0 - 9 keys only
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
        spi_send_data(0, REG_DIGIT1, code_table[pressed_key] + CODE_DP);
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

        /* spi_send_data(0, REG_DIGIT0, CODE_BLANK);
        spi_send_data(0, REG_DIGIT1, CODE_BLANK);
        spi_send_data(0, REG_DIGIT2, CODE_BLANK);
        spi_send_data(0, REG_DIGIT3, CODE_BLANK); */

        clear_display(0, 0);
        add_alarm_in_ms(800, display_program, NULL, false); // 800 ms blank, then display program

        return KEY_STAT_PRG_CHANGE; // program change
    //Insert 1 reg.
    } else {
        return KEY_STAT_NO_CHANGE;
    }
}

uint8_t read_number(uint8_t pressed_key) {
    uint8_t disp = (request_num <= READ_NUM_DISP_1)? 0 : 1;

    if(strlen(inserted_num_buf) == 0) {
        if(pressed_key <= 9) {
            inserted_num_buf[0] = pressed_key + 0x30;
            // printf("%c, Strlen: %d\n", inserted_num_buf[0], strlen(inserted_num_buf));
            if(request_num == READ_NUM_DISP_1) {
                spi_send_data(0, REG_DIGIT4, code_table[pressed_key]);
            } else if(request_num == READ_NUM_DISP_2) {
                spi_send_data(1, REG_DIGIT0, code_table[pressed_key]);
            } else {
                spi_send_data(1, REG_DIGIT4, code_table[pressed_key]);
            }
        } else if(pressed_key == '-') {
            inserted_num_buf[0] = pressed_key;
            // printf("%c, Strlen: %d\n", inserted_num_buf[0], strlen(inserted_num_buf));
            if(request_num == READ_NUM_DISP_1) {
                spi_send_data(0, REG_DIGIT4, code_table[26]);
            } else if(request_num == READ_NUM_DISP_2) {
                spi_send_data(1, REG_DIGIT0, code_table[26]);
            } else {
                spi_send_data(1, REG_DIGIT4, code_table[26]);
            }
        } else if(pressed_key == '.') {
            inserted_num_buf[0] = pressed_key;
            // printf("%c, Strlen: %d\n", inserted_num_buf[0], strlen(inserted_num_buf));
            if(request_num == READ_NUM_DISP_1) {
                spi_send_data(0, REG_DIGIT4, code_table[0] + CODE_DP);
            } else if(request_num == READ_NUM_DISP_2) {
                spi_send_data(1, REG_DIGIT0, code_table[0] + CODE_DP);
            } else {
                spi_send_data(1, REG_DIGIT4, code_table[0] + CODE_DP);
            }
        }

        return KEY_STAT_NUM_INSERTING;
    } else if(strlen(inserted_num_buf) == INSERTED_NUM_BUF_SIZE - 1) {
        if(pressed_key == KEY_ENTER) {
            char *buf;
            inserted_num_buf[strlen(inserted_num_buf)] = '\0';
            inserted_num[request_num - 1] = strtof(inserted_num_buf, &buf);

            // printf("Inserted! String is: %s\n Num is: %f\nChars is: ", inserted_num_buf, inserted_num[request_num - 1]);

            for(register uint8_t i = 0; i <= strlen(inserted_num_buf); i++) {
                printf("%02hhX ", inserted_num_buf[i]);
            }


            memset(inserted_num_buf, 0, INSERTED_NUM_BUF_SIZE);

            gpio_put(LED_OTR, false);
            // request_num = READ_NUM_INSERTED;
            dots_count = 0;
            return KEY_STAT_NUM_INSERTED;
        } else {
            gpio_put(LED_OTR, true);
        }
    } else {
        if(pressed_key <= 9) {
            inserted_num_buf[strlen(inserted_num_buf)] = pressed_key + 0x30;
            // printf("%c, Strlen: %d\n", inserted_num_buf[strlen(inserted_num_buf) - 1], strlen(inserted_num_buf));
            if(strlen(inserted_num_buf) - dots_count < 5) {
                if(request_num == READ_NUM_DISP_1) {
                    spi_send_data(0, REG_DIGIT4 + strlen(inserted_num_buf) - 1 - dots_count, code_table[pressed_key]);
                } else if(request_num == READ_NUM_DISP_2) {
                    spi_send_data(1, REG_DIGIT0 + strlen(inserted_num_buf) - 1 - dots_count, code_table[pressed_key]);
                } else {
                    spi_send_data(1, REG_DIGIT4 + strlen(inserted_num_buf) - 1 - dots_count, code_table[pressed_key]);
                }
            }

            return KEY_STAT_NUM_INSERTING;
        } else if(pressed_key == '.') {
            inserted_num_buf[strlen(inserted_num_buf)] = pressed_key;
            // printf("%c, Strlen: %d\n", inserted_num_buf[strlen(inserted_num_buf) - 1], strlen(inserted_num_buf));
            if(strlen(inserted_num_buf) - dots_count < 5) {
                char prev_digit = inserted_num_buf[strlen(inserted_num_buf) - 2];
                if(request_num == READ_NUM_DISP_1) {
                    spi_send_data(0, REG_DIGIT4 + strlen(inserted_num_buf) - 2 - dots_count, code_table[prev_digit - 0x30] + CODE_DP);
                } else if(request_num == READ_NUM_DISP_2) {
                    spi_send_data(1, REG_DIGIT0 + strlen(inserted_num_buf) - 2 - dots_count, code_table[prev_digit - 0x30] + CODE_DP);
                } else {
                    spi_send_data(1, REG_DIGIT4 + strlen(inserted_num_buf) - 2 - dots_count, code_table[prev_digit - 0x30] + CODE_DP);
                }
            }

            dots_count++;

            return KEY_STAT_NUM_INSERTING;
        } else if(pressed_key == KEY_ENTER) {
            char *buf;
            inserted_num_buf[strlen(inserted_num_buf)] = '\0';
            inserted_num[request_num - 1] = strtof(inserted_num_buf, &buf);

            // printf("Inserted! String is: %s\n Num is: %f\nChars is: ", inserted_num_buf, inserted_num[request_num - 1]);

            /* for(register uint8_t i = 0; i <= strlen(inserted_num_buf); i++) {
                printf("%02hhX ", inserted_num_buf[i]);
            } */


            memset(inserted_num_buf, 0, INSERTED_NUM_BUF_SIZE);

            // request_num = READ_NUM_INSERTED;
            dots_count = 0;
            return KEY_STAT_NUM_INSERTED;
        }
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

                    // Then evaluate key meaning
                    // If there's request to insert number
                    // keypad_status = (request_num == NO_NUM_TO_READ) ? key_evaluate(keymap[i][j]) : read_number(keymap[i][j]);
                    if((request_num == NO_NUM_TO_READ) || (request_num == READ_NUM_INSERTED))
                        keypad_status = key_evaluate(keymap[i][j]);
                    else
                        keypad_status = read_number(keymap[i][j]);
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

float request_number(uint8_t chip, uint8_t display) {
    if((chip == 0) && (display == 0)) {
        // TODO: err
        return 0;
    } else {
        float out_num = 0;
        display_underscores(chip, display);

        if((chip == 0) && (display == 1)) {
            request_num = READ_NUM_DISP_1;
        } else if((chip == 1) && (display == 0)) {
            request_num = READ_NUM_DISP_2;
        } else {
            request_num = READ_NUM_DISP_3;
        }

        while (keypad_status != KEY_STAT_NUM_INSERTED) {
            sleep_ms(50);
        }

        out_num = inserted_num[request_num - 1];
        request_num = NO_NUM_TO_READ;
        keypad_status = KEY_STAT_NO_CHANGE;

        return out_num;
    }
}