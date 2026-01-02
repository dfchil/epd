#ifndef MODE_SINGLE_H
#define MODE_SINGLE_H

#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/game/terrain.h>
#include <gorgol8/types.h>
#include <gorgol8/game/player.h>

typedef struct single_mode_s {
  terrain_t *terrain;
  int num_players;
  game_player_t players[8];
  int offset_x;
} scene_t;

void scene_demolish(scene_t* scene);

scene_t* scene_construct(int num_players);


enj_mode_t* scene_mode_get(void); 


#endif // MODE_SINGLE_H
