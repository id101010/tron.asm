#ifndef GAME_H
#define GAME_H

#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>
#include"player.h"

// Player definitions
#define PLAYER_COUNT    2
#define PLAYER_WIDTH    0   // Don't change

// Speed definitions
#define SPEED_SLOW      10
#define SPEED_FAST      1
#define SPEED_DEFAULT   (SPEED_FAST)

// Button definitions
#define BTN_START               0
#define BTN_PLAYER_1_LEFT       3
#define BTN_PLAYER_1_RIGHT      2
#define BTN_PLAYER_2_LEFT       1
#define BTN_PLAYER_2_RIGHT      0

// Display definitions
#define TFT_GAME_FIELD_TOP              20
#define TFT_GAME_FIELD_BOTTOM           5
#define TFT_GAME_FIELD_LEFT             5
#define TFT_GAME_FIELD_RIGHT            5
#define TFT_GAME_FIELD_START_OFFSET     10
#define TFT_GAME_HEADER_TIME_X          5
#define TFT_GAME_HEADER_TIME_Y          3
#define TFT_GAME_HEADER_PLAYER_X        100
#define TFT_GAME_HEADER_PLAYER_Y        3
#define TFT_GAME_HEADER_PLAYER_WIDTH    100

/**
 * @brief Game data type which contains all game data and players.
 *
 */
typedef struct game_s{
    //public section ahead
    
    uint16_t time;              // Seconds since game start
    uint16_t ticks_per_sec;     // Number of game ticks per second
    int8_t winner_id;           // Player who won the previous round
    uint8_t ticks_per_pixel;    // Number of pixels you a player moves per tick
    player_t player[PLAYER_COUNT];

    enum{               // Current state of the game
        prestart,
        running,
        ended
    } state;

    //private section ahead
    uint8_t ticks_leftover;     // Ticks left to complete a second
    uint8_t ticks_sum_sec;      // Used to calculate the game time
} game_t;

/**
    @brief Initializes the game object
    @param game             Game object
    @param ticks_per_sec    Number of game ticks per second
*/
void game_init(game_t* game, uint16_t ticks_per_sec);

/**
 *   @brief Calculates one step of the game
 *
 *   @param game        Game to calculate a step for
 *   @param deltaTime   Time that passed since the last call to this method (in ticks)
 *   @return            true if the next call to this method should be made with a delta time of zero.
 */
bool game_step(game_t* game, uint64_t deltaTime);

#endif /* GAME_H */
