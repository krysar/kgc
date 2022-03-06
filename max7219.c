/*
 * Library for two MAX72xx LED displays drivers
 * Functionality is limited for exactly two MAX72xx, 7-segment displays only.
 * PINs are hardcoded (spi0 wth GP5, GP17, GP18 and GP19)
 */

#include <math.h>
#include <stdlib.h>

#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "max7219.h"

void cs_select(uint8_t chip) {
    asm volatile("nop \n nop \n nop");
    if(chip == 0)
        gpio_put(PIN_CS1, 0); // Active low
    else if(chip == 1)
        gpio_put(PIN_CS2, 0);
    else
        return;
    asm volatile("nop \n nop \n nop");
}

void cs_deselect(uint8_t chip) {
    asm volatile("nop \n nop \n nop");
    if(chip == 0)
        gpio_put(PIN_CS1, 1);
    else if(chip == 1)
        gpio_put(PIN_CS2, 1);
    else
        return;
    asm volatile("nop \n nop \n nop");
}

void spi_send_data(uint8_t chip, uint8_t address, uint8_t data) {
    // Create an array, put address and data into it
    uint8_t buf[2];
    buf[0] = address;
    buf[1] = data;
    // Shift out the array
    cs_select(chip);
    spi_write_blocking(SPI_PORT, buf, 2);
    cs_deselect(chip);
}

void max7219_init() {
    spi_init(SPI_PORT, SPI_BAUDRATE);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_2pins_with_func(PIN_MOSI, PIN_SCK, GPIO_FUNC_SPI))
    gpio_init(PIN_CS1);
    gpio_set_dir(PIN_CS1, GPIO_OUT);
    gpio_put(PIN_CS1, 1);
    bi_decl(bi_1pin_with_name(PIN_CS1, "SPI CS")); // 1st MAX7219 CS pin
    gpio_init(PIN_CS2);
    gpio_set_dir(PIN_CS2, GPIO_OUT);
    bi_decl(bi_1pin_with_name(PIN_CS2, "SPI CS")); // 2nd MAX7219 CS pin (yes, we don't use cascading)

    // Init both MAX72xx
    for (register uint8_t i = 0; i < 2; i++) {
        spi_send_data(i, REG_DISPLAYTEST, 0x00);
        // Set max intensity
        spi_send_data(i, REG_INTENSITY, 0x0F);
        // Set scan limit register to max
        spi_send_data(i, REG_SCANLIMIT, 0x07);
        // We have own code table
        spi_send_data(i, REG_DECODEMODE, 0x00);
        clear_display(i, 0);
        clear_display(i, 1);
        // Activate shutdown mode
        spi_send_data(i, REG_SHUTDOWN, 0x00);
    }
}

void clear_display(uint8_t chip, uint8_t display) {
    if((chip == 0) || (chip == 1)) {
        if (display == 0) {
            for (register uint8_t i = 1; i <= 4; i++) {
                spi_send_data(chip, i, CODE_BLANK);
            }
        } else if (display == 1) {
            for (register uint8_t i = 5; i <= 8; i++) {
                spi_send_data(chip, i, CODE_BLANK);
            }
        } else
            return;
    } else
        return;
}

uint16_t dec2bin(uint8_t n) {
    uint16_t bin = 0;
    uint16_t rem, i = 1;

    while (n!=0) {
        rem = n % 2;
        n = n >> 1;
        bin += rem * i;
        i *= 10;
    }

    return bin;
}

void display_number(uint8_t chip, uint8_t display, int64_t number) {
    if((display < 0) || (display > 1))
        return; // invalid display number
    else {
        uint8_t num_digits = (number == 0) ? 1 : (uint8_t) log10((double) llabs(number)) + 1; // Calc number of digits
        uint8_t decimal_points[] = {0, 0, 0, 0}; // We're going to use decimal points as exponent

        if (number > 0) { // Display positive number
            uint8_t digits[4];
            for (register uint8_t i = num_digits; i > 0; i--) {
                if (i <= 4) {
                    digits[i - 1] = number % 10; // We're going to display first 4 digits of the number
                }
                number /= 10;
            }

            if (num_digits > 4) {
                uint16_t dps = dec2bin(num_digits - 4);
                for (register uint8_t i = 4; i > 0; i--) {
                    decimal_points[i - 1] = dps % 10; // Rest of number will be expressed by 10^x
                    dps /= 10;
                }
            }

            // Display number depending on  number of digits
            switch (num_digits) {
                case 1:
                    spi_send_data(chip, REG_DIGIT0 + (display * 4), code_table[0]);
                    spi_send_data(chip, REG_DIGIT1 + (display * 4), code_table[0]);
                    spi_send_data(chip, REG_DIGIT2 + (display * 4), code_table[0]);
                    spi_send_data(chip, REG_DIGIT3 + (display * 4), code_table[digits[0]]);
                    break;
                case 2:
                    spi_send_data(chip, REG_DIGIT0 + (display * 4), code_table[0]);
                    spi_send_data(chip, REG_DIGIT1 + (display * 4), code_table[0]);
                    spi_send_data(chip, REG_DIGIT2 + (display * 4), code_table[digits[0]]);
                    spi_send_data(chip, REG_DIGIT3 + (display * 4), code_table[digits[1]]);
                    break;
                case 3:
                    spi_send_data(chip, REG_DIGIT0 + (display * 4), code_table[0]);
                    spi_send_data(chip, REG_DIGIT1 + (display * 4), code_table[digits[0]]);
                    spi_send_data(chip, REG_DIGIT2 + (display * 4), code_table[digits[1]]);
                    spi_send_data(chip, REG_DIGIT3 + (display * 4), code_table[digits[2]]);
                    break;
                default:
                    spi_send_data(chip, REG_DIGIT0 + (display * 4),
                                  code_table[digits[0]] + decimal_points[0] * CODE_DP);
                    spi_send_data(chip, REG_DIGIT1 + (display * 4),
                                  code_table[digits[1]] + decimal_points[1] * CODE_DP);
                    spi_send_data(chip, REG_DIGIT2 + (display * 4),
                                  code_table[digits[2]] + decimal_points[2] * CODE_DP);
                    spi_send_data(chip, REG_DIGIT3 + (display * 4),
                                  code_table[digits[3]] + decimal_points[3] * CODE_DP);
            }

        } else if (number == 0) { // Direct 0 display
            spi_send_data(chip, REG_DIGIT0 + (display * 4), code_table[0]);
            spi_send_data(chip, REG_DIGIT1 + (display * 4), code_table[0]);
            spi_send_data(chip, REG_DIGIT2 + (display * 4), code_table[0]);
            spi_send_data(chip, REG_DIGIT3 + (display * 4), code_table[0]);
        } else { // Display positive number
            if (number < -999999999999999999) { // we're not able to display number <= 10*10^17
                spi_send_data(chip, REG_DIGIT0 + (display * 4), code_table[25]);
                spi_send_data(chip, REG_DIGIT1 + (display * 4), CODE_BLANK);
                spi_send_data(chip, REG_DIGIT2 + (display * 4), code_table[17]);
                spi_send_data(chip, REG_DIGIT3 + (display * 4), code_table[19]);
            } else {
                number = llabs(number);
                uint8_t digits[3];
                for (register uint8_t i = num_digits; i > 0; i--) {
                    if (i <= 3) {
                        digits[i - 1] = number % 10;
                    }
                    number /= 10;
                }

                if (num_digits > 3) {
                    uint16_t dps = dec2bin(num_digits - 3);
                    for (register uint8_t i = 4; i > 0; i--) {
                        decimal_points[i - 1] = dps % 10;
                        dps /= 10;
                    }
                }

                // Display number depending on  number of digits
                switch (num_digits) {
                    case 1:
                        spi_send_data(chip, REG_DIGIT0 + (display * 4), code_table[25]);
                        spi_send_data(chip, REG_DIGIT1 + (display * 4), code_table[0]);
                        spi_send_data(chip, REG_DIGIT2 + (display * 4), code_table[0]);
                        spi_send_data(chip, REG_DIGIT3 + (display * 4), code_table[digits[0]]);
                        break;
                    case 2:
                        spi_send_data(chip, REG_DIGIT0 + (display * 4), code_table[25]);
                        spi_send_data(chip, REG_DIGIT1 + (display * 4), code_table[0]);
                        spi_send_data(chip, REG_DIGIT2 + (display * 4), code_table[digits[0]]);
                        spi_send_data(chip, REG_DIGIT3 + (display * 4), code_table[digits[1]]);
                        break;
                    default:
                        spi_send_data(chip, REG_DIGIT0 + (display * 4), code_table[25] + decimal_points[0] * CODE_DP);
                        spi_send_data(chip, REG_DIGIT1 + (display * 4), code_table[digits[0]] + decimal_points[1] * CODE_DP);
                        spi_send_data(chip, REG_DIGIT2 + (display * 4), code_table[digits[1]] + decimal_points[2] * CODE_DP);
                        spi_send_data(chip, REG_DIGIT3 + (display * 4), code_table[digits[2]] + decimal_points[3] * CODE_DP);
                }
            }
        }
    }
}

//Function displays 2-digit number on one half of the display
void display_two_digit_number(uint8_t chip, uint8_t display, uint8_t half, uint8_t number) {
    if (number > 99) {
        return; // We're not able to display numb. > 99
    } else if ((number < 10)) { // Direct number's display if it's < 10
        spi_send_data(chip, REG_DIGIT0 + 2 * half + 4 * display, code_table[0]);
        spi_send_data(chip, REG_DIGIT1 + 2 * half + 4 * display, code_table[number]);
    } else {
        uint8_t digits[2];

        // Split number into digits
        for (register uint8_t i = 2; i > 0; i--) {
            digits[i - 1] = number % 10;
            number /= 10;
        }

        // Display number
        spi_send_data(chip, REG_DIGIT0 + 2 * half + 4 * display, code_table[digits[0]]);
        spi_send_data(chip, REG_DIGIT1 + 2 * half + 4 * display, code_table[digits[1]]);
    }
}