#include "pico/stdlib.h"

#include "errors.h"
#include "max7219.h"


void display_err_code(uint16_t err_code) {
    clear_display(0, 1);

    spi_send_data(1, REG_DIGIT0, code_table[14]); // E
    spi_send_data(1, REG_DIGIT1, code_table[22]); // r
    spi_send_data(1, REG_DIGIT2, code_table[22]); // r
    spi_send_data(1, REG_DIGIT3, CODE_BLANK);

    display_number(1, 1, err_code);
}