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
#define UART_EMPTY_STRING "0;0;0;0;0;0;0;0;0;0;0;0"
#define UART_SEND_PERIOD_MS 500

#define NUM_BASE_DEC 10
#define NUM_BASE_HEX 16

// Global string for storing data from UART.
extern char uart_str_in[UART_STR_IN_BUF_SIZE];

// Time since last data received in sec.
// Handled by led_update() and led_blinker() in led.c (I know it's not ideal...)
// CON LED starts blinking when connection is probably lost.
// Reseted every time when uart_irq_handler() is triggered.
extern uint8_t time_since_last_connection;

typedef struct ksp_data {
    int64_t num1;
    int64_t num2;
    int64_t num3;
    int8_t fld;
    int8_t elc;
    int8_t mop;
    int8_t lfo;
    int8_t otr;
    int8_t sas;
    int8_t rcs;
    int8_t gea;
    int8_t brk;
    int8_t pau;
} KSP_DATA;

void conn_init();
void uart_irq_handler();
bool uart_handshake();
bool uart_handshake_timer_handler(__unused struct repeating_timer *t);
KSP_DATA uart_data_decoder(char *uart_str);

#endif //KGC_CONNECTION_H