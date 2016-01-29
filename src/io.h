#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stm32f4xx.h>

#include "player.h"


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

/**
 *   @brief Read a value from the analog/digital converter.
 *
 *   @return            ADC_value as an unsigned int16
 */
uint16_t read_adc();


#define ADC_MASK 0x3FF
#define ADC_MAX 0x3A0
#define ADC_TOLERANCE 0x08


/**
 *   @brief Read the values of the 8 switches
 *
 *   @return Returns the state of the switches, one bit per switch, high active
 */
uint8_t read_switches();


#endif /* IO_H */
