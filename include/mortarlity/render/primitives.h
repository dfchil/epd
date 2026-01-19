#ifndef RENDER_PRIMITIVES_H
#define RENDER_PRIMITIVES_H

#include <dc/pvr.h>
#include <sh4zam/shz_sh4zam.h>

SHZ_INLINE shz_vec3_t render_perspective_div(shz_vec4_t v) {
  const float inv_w = shz_invf_fsrra(v.w);
  return shz_vec3_init(v.x * inv_w, v.y * inv_w, inv_w);
}

void render_strip_line(shz_vec2_t* points, int point_count, shz_vec3_t* offset,
                       float line_width, enj_color_t color, pvr_list_t list,
                       float* zvalues);

#endif // RENDER_PRIMITIVES_H