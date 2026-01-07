#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/game/shell.h>
#include <mortarlity/render/player.h>

static const enj_color_t _player_colors[] = {
    {.raw = 0xff14a5ff}, // blue
    {.raw = 0xffffc010}, // light orange/gold
    {.raw = 0xffff450a}, // red
    {.raw = 0xff0fff50}, // neon green
    {.raw = 0xff9c27ff}, // Purple
    {.raw = 0xffc9c0bb}, // silver
    {.raw = 0xffff6ec7}, // neon pink
    {.raw = 0xff009b7d}, // teal
};
#define NUM_PLAYER_COLORS (sizeof(_player_colors) / sizeof(enj_color_t))

#define ANGLE_MAX 2.26893  // 130 degrees in radians
#define ANGLE_MIN 0.872665 // 50 degrees in radians

// static const char* _player_color_names[NUM_PLAYER_COLORS] = {
//     "BLUE", "GOLD", "RED ", "GREEN", "PURPLE", "SILVER", "PINK", "TEAL"};

enj_color_t player_color_get(int player_index) {
  return _player_colors[player_index % NUM_PLAYER_COLORS];
}

void player_initialize(int player_index, void *scene) {
  game_player_t *player = &((scene_t *)scene)->players[player_index];
  player->position =
      ((scene_t *)scene)->terrain->player_positions[player_index];
  player->health = 100;
  player->color = _player_colors[player_index % NUM_PLAYER_COLORS];
  player->shoot_angle =
      ANGLE_MIN + (ANGLE_MAX - ANGLE_MIN) * player_index /
                      (NUM_PLAYER_COLORS - 1); // point in different directions
  player->shoot_power = (MAX_SHOOT_POWER - MIN_SHOOT_POWER) * 0.5f +
                        MIN_SHOOT_POWER;
  player->cooldown_timer = SHOT_COOLDOWN_FRAMES;
  player->scene = scene;
  player->controller.updatefun = NULL;
  player->controller.state = NULL;
}

int player_update(game_player_t *player) {
  // do good stuff here in future
  if (player->health <= 0) {
    player->health = 0;
    return 0; // player is dead
  }
  if (player->cooldown_timer > 0) {
    player->cooldown_timer--;
  }
  enj_ctrlr_state_t state = {0};
  if (player->controller.updatefun && player->controller.state) {
    enj_read_controller(&player->controller, &state);
  }
  if (player->cooldown_timer <= 0 && state.button.A == ENJ_BUTTON_DOWN) {
    // shoot
    player->cooldown_timer = SHOT_COOLDOWN_FRAMES; // 1 second cooldown

    shell_create(player->position.x, player->position.y,
                 cosf(player->shoot_angle) * player->shoot_power * 10.0f,
                 sinf(player->shoot_angle) * player->shoot_power * 10.0f,
                 player);
  }
  if (state.button.UP == ENJ_BUTTON_DOWN) {
    player->shoot_power += 0.5f;
  }
  if (state.button.DOWN == ENJ_BUTTON_DOWN) {
    player->shoot_power -= 0.5f;
  }
  if (state.button.LEFT == ENJ_BUTTON_DOWN) {
    player->shoot_angle += 0.01f;
    if (player->shoot_angle > ANGLE_MAX) {
      player->shoot_angle = ANGLE_MAX;
    }
  }
  if (state.button.RIGHT == ENJ_BUTTON_DOWN) {
    player->shoot_angle -= 0.01f;
  }
  if (state.joyx < -10) {
    player->shoot_angle += 0.001f * -((float)state.joyx) / 128.0f;
    if (player->shoot_angle > ANGLE_MAX) {
      player->shoot_angle = ANGLE_MAX;
    }
  } else if (state.joyx > 10) {
    player->shoot_angle -= 0.001f * ((float)state.joyx) / 128.0f;
  }
  if (state.joyy > 10) {
    player->shoot_power -= 0.05f * ((float)state.joyy) / 128.0f;
  } else if (state.joyy < -10) {
    player->shoot_power += 0.05f * -((float)state.joyy) / 128.0f;
  }

  if (player->shoot_angle < ANGLE_MIN) {
    player->shoot_angle = ANGLE_MIN;
  }
  if (player->shoot_angle > ANGLE_MAX) {
    player->shoot_angle = ANGLE_MAX;
  }
  if (player->shoot_power > MAX_SHOOT_POWER) {
    player->shoot_power = MAX_SHOOT_POWER;
  }
  if (player->shoot_power < MIN_SHOOT_POWER) {
    player->shoot_power = MIN_SHOOT_POWER;
  }

  return 1;
}
