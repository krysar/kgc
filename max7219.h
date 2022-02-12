#ifndef KGC_MAX7219_H
#define KGC_MAX7219_H

#define PIN_MOSI 19 // DIN
#define PIN_SCK 18 // CLK
#define PIN_CS1 17 // LOAD (CS)
#define PIN_CS2 5 // LOAD (CS)
#define SPI_PORT spi0

#define REG_NOOP        0x00
#define REG_DIGIT0      0x01
#define REG_DIGIT1      0x02
#define REG_DIGIT2      0x03
#define REG_DIGIT3      0x04
#define REG_DIGIT4      0x05
#define REG_DIGIT5      0x06
#define REG_DIGIT6      0x07
#define REG_DIGIT7      0x08
#define REG_DECODEMODE  0x09
#define REG_INTENSITY   0x0A
#define REG_SCANLIMIT   0x0B
#define REG_SHUTDOWN    0x0C
#define REG_DISPLAYTEST 0x0D

#define CODE_BLANK      0x00
#define CODE_DP         0x80
static uint8_t code_table[] = {0x7E, 0x30, 0x6D, 0x79,     // 0, 1, 2, 3
                        0x33, 0x5B, 0x5F, 0x70,            // 4, 5, 6, 7
                        0x7F, 0x7B, 0x77, 0x1F,            // 8, 9, A, b
                        0x4E, 0x3D, 0x4F, 0x47,            // C, d, E, F
                        0x17, 0x0E, 0x15, 0x1D,            // h, L, n, o
                        0x67, 0x05, 0x07, 0x1C,            // P, r, t, u
                        0x08, 0x01};                       // _, -

void cs_select(uint8_t chip);
void cs_deselect(uint8_t chip);
void spi_send_data(uint8_t chip, uint8_t address, uint8_t data);
void max7219_init();
void clear_display(uint8_t chip);
uint16_t dec2bin(uint8_t n);
void display_number(uint8_t chip,uint8_t display, int64_t number);
void display_two_digit_number(uint8_t chip, uint8_t display, uint8_t half, uint8_t number);

#endif //KGC_MAX7219_H
