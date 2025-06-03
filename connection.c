/*
 * Library for communication with server via UART
 * PINs are hardcoded (0 for RXD, 1 for TXD)
*/

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#include "connection.h"
#include "max7219.h"
#include "led.h"

char uart_str_in[UART_STR_IN_BUF_SIZE] = "";

void conn_init() {
    uart_init(UART_ID, UART_BAUD_RATE);

    gpio_set_function(UART_TXD_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TXD_PIN));
    gpio_set_function(UART_RXD_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RXD_PIN));

    // Set format
    uart_set_format(UART_ID, UART_DATA_BITS, UART_STOP_BITS, UART_PARITY_EVEN);

    // We haven't CTS/RTS PINs, so...
    uart_set_hw_flow(UART_ID, false, false);

    // Turn off FIFO
    uart_set_fifo_enabled(UART_ID, false);

    // Set up RXD interrupt
    int uart_irq = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(uart_irq, uart_irq_handler);
    irq_set_enabled(uart_irq, true);
    uart_set_irq_enables(UART_ID, true, false);
    
}

void uart_irq_handler() {
    char uart_str_in_buf[UART_STR_IN_BUF_SIZE] = "";
    // memset(uart_str_in, 0, UART_STR_IN_BUF_SIZE);
    printf("Incomming message: ");
    while (uart_is_readable_within_us(UART_ID, 850)) {
        char ch_buf = uart_getc(UART_ID);
        printf("%c", ch_buf);
        uart_str_in_buf[strlen(uart_str_in_buf)] = ch_buf;
        /*if(ch_buf == '\n')
            break;*/
    }
    
    if(strlen(uart_str_in_buf) > 0) {
        memset(uart_str_in, 0, UART_STR_IN_BUF_SIZE);
        strcpy(uart_str_in, uart_str_in_buf);
        uart_str_in[strlen(uart_str_in)] = '\0';
    }
    
    printf("str: %s\n", uart_str_in);
    printf("strlen: %d\n", strlen(uart_str_in));
    printf("\n");
}

bool uart_handshake() {
    // Display message about connecting
    spi_send_data(0, REG_DIGIT0, code_table[12]); // c
    spi_send_data(0, REG_DIGIT1, code_table[20]); // o
    spi_send_data(0, REG_DIGIT2, code_table[19]); // n
    spi_send_data(0, REG_DIGIT3, code_table[19]); // n
    clear_display(0, 1);
    clear_display(1, 0);
    clear_display(1, 1);

    // We're going to send handshake message every 500 ms until answer is received
    struct repeating_timer handshake_timer;
    add_repeating_timer_ms(500, uart_handshake_timer_handler, NULL, &handshake_timer);

    // Waiting for answer
    while(strlen(uart_str_in) == 0) {
        printf("Stucked :/\n");
    }

    if(strcmp(uart_str_in, UART_HANDSHAKE_ANSWER) == 0) {
        cancel_repeating_timer(&handshake_timer);
        gpio_put(LED_CON, true);
        return 0;
    } else {
        cancel_repeating_timer(&handshake_timer);
        return 1;
    }
}

bool uart_handshake_timer_handler(__unused struct repeating_timer *t) {
    char handshake_message[] = UART_HANDSHAKE_MESSAGE;
    uart_puts(UART_ID, handshake_message);
}