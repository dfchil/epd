#ifndef RENDER_PRIMITIVES_H
#define RENDER_PRIMITIVES_H

#include <sh4zam/shz_sh4zam.h>
#include <dc/pvr.h>

void render_sprite_line(shz_vec2_t from, shz_vec2_t to, float zvalue, float line_width, pvr_dr_state_t *state_ptr);


#endif // RENDER_PRIMITIVES_H