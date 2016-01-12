#ifndef PLAYER_H
#define PLAYER_H

#include<stdint.h>
#include<stdlib.h>
#include"player.h"

#define PLAYER_COUNT 2

typedef struct game_s{
    uint16_t time;          // seconds since game start
    uint8_t winner_id;
    uint8_t host_id;
    player_t player[PLAYER_COUNT];

    enum{
        running,
        ended
    } state;

} game_t;

void game_init(game_t* game);
void game_step(game_t* game, uint16_t time_delta);

#endif /* PLAYER_H */
