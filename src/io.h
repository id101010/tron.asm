#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stm32f4xx.h>


/**
 *   @brief Initialize all used GPIOs and initialize their clock source.
 */
void io_init(void);

/**
 *   @brief Edge detection for the buttons which were initialized by io_init.
 *          Gets called by the systick timer.
 */
void io_process(void);

/**
 *   @brief Edge handler which clears the edge bit to make sure a button press gets only handled once per tick.
 *
 *   @param btnnumber   Button number
 *   @return            True if the button has a positive edge.
 */
bool io_button_has_edge(uint8_t btnnumber);

#endif /* IO_H */
