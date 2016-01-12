#ifndef UART_H
#define UART_H

#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct uart_message_s{
    uint16_t message_length;
    uint8_t *message_data;

    // Internal, do not touch!
    bool __deleted;     
} uart_message_t;

void uart_init(void);
void uart_transmit(uart_message_t *message);            // transmit a message and frees it
bool uart_has_message(void);                            // checks if there is at least one msg available
void uart_message_free(uart_message_t *message);        // frees a message

uart_message_t *uart_message_allocate(void);            // allocate storage for a new message
uart_message_t *receive_message(void);                  // returns a message if available else null

#endif /* UART_H */
