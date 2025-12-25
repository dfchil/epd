#include <enDjinn/enj_enDjinn.h>
#include <sh4zam/shz_sh4zam.h>

void render_sprite_line(shz_vec2_t from, shz_vec2_t to, float zvalue,
                        float line_width, pvr_dr_state_t* state_ptr) {
  shz_vec2_t direction = shz_vec2_normalize(shz_vec2_sub(to, from));

  float corners[4][3] = {
      {from.x, from.y, zvalue},
      {to.x, to.y, zvalue},
      {to.x + direction.y * line_width * ENJ_XSCALE,
       to.y - direction.x * line_width, zvalue},
      {from.x + direction.y * line_width * ENJ_XSCALE,
       from.y - direction.x * line_width, zvalue},
  };
  enj_draw_sprite(corners, state_ptr, NULL, NULL);
}
