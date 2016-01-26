#ifndef GAME_H
#define GAME_H

#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>
#include"player.h"

#define PLAYER_COUNT    2
#define PLAYER_WIDTH    0   // Don't change 
#define SPEED_SLOW      10
#define SPEED_FAST      1
#define SPEED_DEFAULT   (SPEED_FAST)

#define BTN_START               0
#define BTN_PLAYER_1_LEFT       3
#define BTN_PLAYER_1_RIGHT      2
#define BTN_PLAYER_2_LEFT       1
#define BTN_PLAYER_2_RIGHT      0

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

typedef struct game_s{
    //public section
    
    uint16_t time;          // seconds since game start
    uint16_t ticks_per_sec;
    int8_t winner_id;
    uint8_t ticks_per_pixel;
    player_t player[PLAYER_COUNT];

    enum{
        prestart,
        running,
        ended
    } state;

    //private section ahead:
    uint8_t ticks_leftover;
    uint8_t ticks_sum_sec;
} game_t;

/**
    @brief Initializes the game object
    @param game
    @param ticks_per_sec
*/
void game_init(game_t* game, uint16_t ticks_per_sec);

/**
    @brief Calculates one step of the game
    @param game Game to calculate a step for
    @param deltaTime Time that passed since the last call to this method (in ticks)
    @return true if the next call to this method should be made with a delta time of zero.

*/
bool game_step(game_t* game, uint64_t deltaTime);

#endif /* GAME_H */
