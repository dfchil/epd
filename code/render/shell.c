#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/shell.h>

void _render_shell_TR(void *data) {
  shell_t *shell = (shell_t *)data;
  enj_color_t shell_color = shell->origin->color.contrast;
  enj_color_t trail_color = shell->origin->color.primary;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_TR_POLY);
  pvr_sprite_hdr_t hdr_s;
  pvr_sprite_compile(&hdr_s, &cxt);
  hdr_s.argb = shell_color.raw;

  float offset_x = (float)((scene_t *)shell->origin->scene)->offset_x;

  #define SHELL_SIZE 2.0f
  #define SHELL_Z 3.0f
  if (shell->moving) {
    enj_draw_sprite(
        (float[4][3]){
            {(shell->position.x + offset_x - SHELL_SIZE) * ENJ_XSCALE,
             vid_mode->height - shell->position.y - SHELL_SIZE  , SHELL_Z},
            {(shell->position.x + offset_x + SHELL_SIZE) * ENJ_XSCALE,
             vid_mode->height - shell->position.y - SHELL_SIZE, SHELL_Z},
            {(shell->position.x + offset_x + SHELL_SIZE) * ENJ_XSCALE,
             vid_mode->height - shell->position.y + SHELL_SIZE, SHELL_Z},
            {(shell->position.x + offset_x - SHELL_SIZE) * ENJ_XSCALE,
             vid_mode->height - shell->position.y + SHELL_SIZE, SHELL_Z},
        },
        &state, &hdr_s, NULL);
  }
  pvr_poly_cxt_t cxt_poly;
  pvr_poly_cxt_col(&cxt_poly, PVR_LIST_TR_POLY);
  cxt_poly.gen.culling = PVR_CULLING_NONE;
  pvr_poly_hdr_t *hdr_pp = (pvr_poly_hdr_t *)pvr_dr_target(state);
  pvr_poly_compile(hdr_pp, &cxt_poly);
  pvr_dr_commit(hdr_pp);

#define SHELL_TRAIL_WIDTH 2.0f

  for (int i = 0; i < shell->used_trail_steps; i++) {
    int last_vert_pair = i == shell->used_trail_steps - 1;
    int sprite_pos = shell_modulo(-shell->frame + i, SHELL_MOTION_BLUR_STEPS);
    if (shell->moving) {
      trail_color.a = SHELL_MOTION_BLUR_STEPS - i;
    } else {
      trail_color.a = (uint8_t)(shell->trail_fade - i);
    }
    if (trail_color.a <= 0) {
      trail_color.a = 0;
      last_vert_pair = 1;
    }

    shz_vec2_t *from = shell->trail + sprite_pos;
    shz_vec2_t *to =
        shell->trail + ((sprite_pos + 1) % SHELL_MOTION_BLUR_STEPS);

    shz_vec2_t dir = shz_vec2_normalize(shz_vec2_sub(*to, *from));

    pvr_vertex_t *vert = (pvr_vertex_t *)pvr_dr_target(state);
    vert->flags = PVR_CMD_VERTEX;
    vert->x = (from->x + offset_x + dir.y * SHELL_TRAIL_WIDTH) * ENJ_XSCALE;
    vert->y = vid_mode->height - (from->y - dir.x * SHELL_TRAIL_WIDTH);
    vert->z = 1.0f;
    vert->argb = trail_color.raw;
    pvr_dr_commit(vert);

    vert = (pvr_vertex_t *)pvr_dr_target(state);
    vert->flags = last_vert_pair ? PVR_CMD_VERTEX_EOL : PVR_CMD_VERTEX;
    vert->x = (from->x + offset_x - dir.y * SHELL_TRAIL_WIDTH) * ENJ_XSCALE;
    vert->y = vid_mode->height - (from->y + dir.x * SHELL_TRAIL_WIDTH);
    vert->z = 1.0f;
    vert->argb = trail_color.raw;
    pvr_dr_commit(vert);
    if (last_vert_pair) {
      break;
    }
  }
  pvr_dr_finish();
}

void render_shell(shell_t *shell) {
  enj_render_list_add(PVR_LIST_TR_POLY, _render_shell_TR, (void *)shell);
}