#ifndef PLAYER_H
#define PLAYER_H

#include<stdlib.h>
#include<stdint.h>

#define max_positions 320 // Maximum of points a player object is holding.

/**
 * @brief Direction data type, used to specify the direction when moving.
 */
typedef enum direction_e {
    right,  // going right
    down,   // going down
    left,   // going left
    up      // going up
} direction_t;

/**
 * @brief Point data type which is used to store a location for the LCD.
 */
typedef struct point_s{
    uint16_t x; // x position on the display
    uint8_t y;  // y position on the display
} point_t;

/**
 * @brief Player data type which stores all data for a single player
 */
typedef struct player_s {
    uint8_t btn_left;           // players left-button
    uint8_t btn_right;          // players right-button
    uint16_t color;             // players color
    uint16_t num_positions;     // players number of past edges
    direction_t direction;      // players current moving direction

    enum{                       // players current state
        dead,
        alive,
    } state;

    point_t past_positions[max_positions]; // used to store players waypoints
    point_t position; // current position

} player_t;

/**
 *  @brief Initialize a new player object and fill its data fields.
 *
 *  @param player       Player object  
 *  @param btn_left     Left-button for the new player 
 *  @param btn_right    Right-button for the new player
 *  @param pos          Start position for the new player
 *  @param color        Players color
 *  @param direction    Players moving direction
 */
void player_init(player_t* player, uint8_t btn_left, uint8_t btn_right, point_t pos, uint16_t color, direction_t direction);

/**
 *  @brief Updates num_position and adds current position to the list
 *
 *  @param player       Player object
 *  @param point        Point to add to the player object
 */
void player_append_position(player_t* player, point_t point);

#endif /* PLAYER_H */
