/*
 * Library for communication with main computer via UART
 * PINs are hardcoded (0 for RXD, 1 for TXD)
*/

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#include "connection.h"

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
    while (uart_is_readable_within_us(UART_ID, 800)) {
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
    
    printf("strlen: %d\n", strlen(uart_str_in));
    printf("\n");
    /*register uint8_t i = 0;
    do {
        printf("0x%x ", (unsigned char)uart_str_in[i]);
    } while (uart_str_in[i] != 0); */
    
}
