#include <dc/video.h>
#include <malloc.h>
#include <mortarlity/game/shell.h>
#include <mortarlity/render/shell.h>

static shell_t *first_shell = NULL;
static shell_t *last_shell = NULL;

shell_t *shell_get_first() { return first_shell; }

shell_t *shell_create(float pos_x, float pos_y, float vel_x, float vel_y,
                      game_player_t *origin) {
  shell_t *new_shell = memalign(32, sizeof(shell_t));
  if (!new_shell) {
    return NULL;
  }
  new_shell->position = (shz_vec2_t){.x = pos_x, .y = pos_y};
  new_shell->velocity = (shz_vec2_t){.x = vel_x, .y = vel_y};
  for (int i = 0; i < SHELL_MOTION_BLUR_STEPS; i++) {
    new_shell->trail[i] = new_shell->position;
  }

  new_shell->origin = origin;
  new_shell->frame = 0;

  if (first_shell == NULL) {
    first_shell = new_shell;
    last_shell = new_shell;
    new_shell->prev = NULL;
    new_shell->next = NULL;
  } else {
    last_shell->next = new_shell;
    new_shell->prev = last_shell;
    new_shell->next = NULL;
    last_shell = new_shell;
  }

  return new_shell;
}

//

int shell_update(shell_t *shell, float delta_time) {

  shell->frame++;
  uint32_t current_frame = shell->frame % SHELL_MOTION_BLUR_STEPS;

  const float time_step = delta_time / (float)SHELL_SIMULATION_STEPS;
  for (int i = 0; i < SHELL_SIMULATION_STEPS; i++) {
    shell->position.x += shell->velocity.x * time_step;
    shell->position.y += shell->velocity.y * time_step;
    shell->velocity.y -= 9.81f * time_step; // gravity
  }
  shell->trail[current_frame] = shell->position;

  // Check if the shell has fallen below y = 0 (ground level)
  if (shell->position.y < 0.0f) {
    return 0; // Indicate that the shell should be destroyed
  }
  if (shell->position.x < 0.0f || shell->position.x > vid_mode->width) {
    return 0; // Indicate that the shell should be destroyed
  }

  render_shell(shell);
  return 1; // Shell is still active
}

void shell_destroy(shell_t *shell) {
  if (shell->prev) {
    shell->prev->next = shell->next;
  } else {
    first_shell = shell->next;
  }
  if (shell->next) {
    shell->next->prev = shell->prev;
  } else {
    last_shell = shell->prev;
  }
  free(shell);
}
