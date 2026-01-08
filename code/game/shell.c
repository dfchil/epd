#include <dc/video.h>
#include <malloc.h>
#include <mortarlity/game/shell.h>
#include <mortarlity/render/shell.h>

static shell_t* first_shell = NULL;
static shell_t* last_shell = NULL;


inline int shell_modulo(int x,int N){
    return (x % N + N) %N;
}

shell_t* shell_get_first() { return first_shell; }

shell_t* shell_create(float pos_x, float pos_y, float vel_x, float vel_y,
                      game_player_t* origin) {
  shell_t* new_shell = memalign(32, sizeof(shell_t));
  if (!new_shell) {
    return NULL;
  }
  new_shell->position = (shz_vec2_t){.x = pos_x, .y = pos_y};
  new_shell->velocity = (shz_vec2_t){.x = vel_x, .y = vel_y};
  for (int i = 0; i < SHELL_MOTION_BLUR_STEPS; i++) {
    new_shell->trail[i].x = 0.0f;
    new_shell->trail[i].y = 0.0f;
  }

  new_shell->origin = origin;
  new_shell->trail[SHELL_MOTION_BLUR_STEPS -1 ] = new_shell->position;
  new_shell->frame = 1;
  new_shell->used_trail_steps = 1;
  new_shell->trail_fade = 0;
  new_shell->moving = 1;

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

int shell_update(shell_t* shell, float delta_time) {
  if (!shell->moving) {
    shell->trail_fade--;
    if (shell->trail_fade == 0) {
      return 0;  // Indicate that the shell should be destroyed
    }
    render_shell(shell);
    return 1;  // Shell is inactive but not destroyed
  }
  if (shell->used_trail_steps < SHELL_MOTION_BLUR_STEPS) {
    shell->used_trail_steps++;
  }
  
  shell->frame++;
  // move backwards in the trail buffer
  int32_t current_frame = shell_modulo(-shell->frame, SHELL_MOTION_BLUR_STEPS);

  const float time_step = delta_time / (float)SHELL_SIMULATION_STEPS;
  for (int i = 0; i < SHELL_SIMULATION_STEPS; i++) {
    shell->position.x += shell->velocity.x * time_step;
    shell->position.y += shell->velocity.y * time_step;
    shell->velocity.y -= 9.81f * time_step;  // gravity
  }
  shell->trail[current_frame] = shell->position;

  // Check if the shell has fallen below y = 0 (ground level)
  if (shell->position.y < 0.0f) {
    shell->trail_fade = SHELL_MOTION_BLUR_STEPS;
    shell->moving = 0;
  }
  if (shell->position.x < 0 || shell->position.x > vid_mode->width) {
    shell->trail_fade = SHELL_MOTION_BLUR_STEPS;
    shell->moving = 0;
  }
  render_shell(shell);
  return 1;  // Shell is still active
}

void shell_destroy(shell_t* shell) {
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
