#ifndef RENDER_PRIMITIVES_H
#define RENDER_PRIMITIVES_H

#include <sh4zam/shz_sh4zam.h>
#include <dc/pvr.h>

void render_sprite_line(shz_vec2_t from, shz_vec2_t to, float zvalue, float line_width, pvr_dr_state_t *state_ptr);

void render_strip_line(shz_vec2_t *points, int point_count, float zvalue, float line_width, enj_color_t color, pvr_list_t list);

#endif // RENDER_PRIMITIVES_H