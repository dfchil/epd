#ifndef RENDER_PRIMITIVES_H
#define RENDER_PRIMITIVES_H

#include <dc/pvr.h>
#include <sh4zam/shz_sh4zam.h>

SHZ_INLINE shz_vec3_t render_perspective_div(shz_vec4_t v) { return v.xyz; }

void render_strip_line_col(shz_vec2_t *points, int point_count, shz_vec3_t *offset,
                       float line_width, enj_color_t color, pvr_list_t list,
                       float *zvalues);

#endif // RENDER_PRIMITIVES_H