#include <mortarlity/game/player.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/game/shell.h>
#include <mortarlity/render/player.h>

static player_col_def_t _player_colors[] = {
    {.primary = {.raw = 0xff14a5ff}, .contrast = {.raw = 0}}, // blue
    {.primary = {.raw = 0xffffc010},
     .contrast = {.raw = 0}}, // light orange/gold
    {.primary = {.raw = 0xffff450a}, .contrast = {.raw = 0}}, // red
    {.primary = {.raw = 0xff0fff50}, .contrast = {.raw = 0}}, // neon green
    {.primary = {.raw = 0xff9c27ff}, .contrast = {.raw = 0}}, // Purple
    {.primary = {.raw = 0xffc9c0bb}, .contrast = {.raw = 0}}, // silver
    {.primary = {.raw = 0xffff6ec7}, .contrast = {.raw = 0}}, // neon pink
    {.primary = {.raw = 0xff009b7d}, .contrast = {.raw = 0}}, // teal
};

#define NUM_PLAYER_COLORS (sizeof(_player_colors) / sizeof(_player_colors[0]))

#define ANGLE_MAX 2.26893  // 130 degrees in radians
#define ANGLE_MIN 0.872665 // 50 degrees in radians
#define SHOT_POWER_BIG_STEP (MAX_SHOOT_POWER / 40.0f)
#define SHOT_POWER_SMALL_STEP (MAX_SHOOT_POWER / 200.0f)

#ifndef SHZ_ABS
#define SHZ_ABS(x) ((x) < 0 ? -(x) : (x))
#endif 


// static const char* _player_color_names[NUM_PLAYER_COLORS] = {
//     "BLUE", "GOLD", "RED ", "GREEN", "PURPLE", "SILVER", "PINK", "TEAL"};

player_col_def_t player_color_get(int player_index) {
  return _player_colors[player_index % NUM_PLAYER_COLORS];
}

void player_setup_colors() {
  _player_colors[1].contrast.raw =
      _player_colors[5].primary.raw; // copy silver to gold
  _player_colors[5].contrast.raw =
      _player_colors[1].primary.raw; // copy gold to silver

  for (size_t i = 0; i < NUM_PLAYER_COLORS; i++) {
    enj_color_t negcol;
    if (_player_colors[i].contrast.raw == 0) {
      negcol =
          (enj_color_t){.a = 255,
                        .r = SHZ_MIN(255 - _player_colors[i].primary.r + 48, 255),
                        .g = SHZ_MIN(255 - _player_colors[i].primary.g + 48, 255),
                        .b = SHZ_MIN(255 - _player_colors[i].primary.b + 48, 255)};
    } else {
      negcol = (enj_color_t){.a = 255,
                             .r = SHZ_MIN(_player_colors[i].contrast.r + 48, 255),
                             .g = SHZ_MIN(_player_colors[i].contrast.g + 48, 255),
                             .b = SHZ_MIN(_player_colors[i].contrast.b + 48, 255)};
    }
    _player_colors[i].contrast.raw = negcol.raw;
  }
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
  player->shoot_power =
      (MAX_SHOOT_POWER - MIN_SHOOT_POWER) * 0.5f + MIN_SHOOT_POWER;
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

    const shz_sincos_t barrel = shz_sincosf(player->shoot_angle);
    shell_create(player->position.x + barrel.cos * BARREL_OFFSET,
                 player->position.y + barrel.sin * BARREL_OFFSET,
                 barrel.cos * player->shoot_power,
                 barrel.sin * player->shoot_power, player);
  }
  if (state.button.UP == ENJ_BUTTON_DOWN) {
    player->shoot_power += SHOT_POWER_BIG_STEP;
  }
  if (state.button.DOWN == ENJ_BUTTON_DOWN) {
    player->shoot_power -= SHOT_POWER_BIG_STEP;
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
  if (SHZ_ABS(state.joyx) > 10) {
    player->shoot_angle += 0.001f * -((float)state.joyx) / 128.0f;
  }
  if (SHZ_ABS(state.joyy) > 10) {
    player->shoot_power -= SHOT_POWER_SMALL_STEP * ((float)state.joyy) / 128.0f;
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
