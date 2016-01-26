#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stm32f4xx.h>


void io_init(void);
void io_process(void);
bool io_button_has_edge(uint8_t btnnumber);


#endif /* IO_H */
