#include <mortarlity/game/player.h>
#include <mortarlity/render/player.h>

int player_update(game_player_t* player) {
  // do good stuff here in future
  if (player->health <= 0) {
    player->health = 0;
    return 0;  // player is dead
  }
  enj_ctrlr_state_t state;
  // enj_read_controller(&player->controller, &state);

  return 1;
}
