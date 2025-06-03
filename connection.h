#ifndef KGC_CONNECTION_H
#define KGC_CONNECTION_H

#define UART_ID uart0
#define UART_BAUD_RATE 115200
#define UART_DATA_BITS 8
#define UART_STOP_BITS 1
#define UART_PARITY UART_PARITY_EVEN
#define UART_TXD_PIN 0
#define UART_RXD_PIN 1
#define UART_STR_IN_BUF_SIZE 256
#define UART_HANDSHAKE_MESSAGE "WAITING\n"
#define UART_HANDSHAKE_ANSWER "ACCEPTED\n"

#define NUM_BASE_DEC 10
#define NUM_BASE_HEX 16

extern char uart_str_in[UART_STR_IN_BUF_SIZE];

void conn_init();
void uart_irq_handler();
bool uart_handshake();
bool uart_handshake_timer_handler(__unused struct repeating_timer *t);

#endif //KGC_CONNECTION_H