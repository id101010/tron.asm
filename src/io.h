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
 *   @brief Initialize the analog/digital converter in order to read the potentiometer.
 */
void init_adc();

/**
 *   @brief Read a value from the analog/digital converter.
 *
 *   @return            ADC_value as an unsigned int16
 */
uint16_t read_adc();

/**
 *   @brief Uses buttons s0-7 to determine the coler of the player. 
 *          Player1 is represented by the bits s0-3.
 *          Player2 is represented by the bits s4-s7.
 *
 *   @param player Playerobject
 *   @param first_player Specify first or second player
 *   @return Color value for the according player
 */
uint16_t get_player_color(player_t* player, bool first_player);

#endif /* IO_H */
