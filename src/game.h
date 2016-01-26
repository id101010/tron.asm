#ifndef GAME_H
#define GAME_H

#include<stdint.h>
#include<stdlib.h>
#include"player.h"

#define PLAYER_COUNT    2
#define PLAYER_WIDTH    0   // Don't change 
#define SPEED_SLOW      10
#define SPEED_FAST      1
#define SPEED_DEFAULT   (SPEED_SLOW)

#define BTN_START               0
#define BTN_PLAYER_1_LEFT       3
#define BTN_PLAYER_1_RIGHT      2
#define BTN_PLAYER_2_LEFT       1
#define BTN_PLAYER_2_RIGHT      0

typedef struct game_s{
    //public section
    
    uint16_t time;          // seconds since game start
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
} game_t;

void game_init(game_t* game);
void game_step(game_t* game, uint64_t deltaTime);

#endif /* GAME_H */
