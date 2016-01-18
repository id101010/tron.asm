#include "player.h"
#include <color.h>

void player_init(player_t* player) {
    player->id=0;
    player->color = GUI_COLOR_BLUE;
    player->num_positions=1;
    player->position = (point_t){.x=10,.y=100};
    player->past_positions[0] = player->position;
    player->direction = right;
    player->state = alive;
}




void player_append_position(player_t* player, point_t point) {
    if(player->num_positions < max_positions) {
        player->past_positions[player->num_positions++] = point;
    } 
} 
