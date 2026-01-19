#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/render/primitives.h>
#include <sh4zam/shz_sh4zam.h>

void render_strip_line(shz_vec2_t* points, int point_count, shz_vec3_t* offset,
                       float line_width, enj_color_t color, pvr_list_t list,
                       float* zvalues) {
  shz_xmtrx_init_translation(offset->x * ENJ_XSCALE,
                             vid_mode->height - offset->y, offset->z);
  shz_xmtrx_apply_scale(ENJ_XSCALE, 1.0f, 1.0f);

  pvr_dr_state_t state;
  pvr_dr_init(&state);
  pvr_poly_cxt_t cxt;
  pvr_poly_cxt_col(&cxt, list);
  cxt.gen.culling = PVR_CULLING_NONE;

  pvr_poly_hdr_t* hdr = (pvr_poly_hdr_t*)pvr_dr_target(state);
  pvr_poly_compile(hdr, &cxt);
  pvr_dr_commit(hdr);

  shz_vec2_t v0 = shz_vec2_normalize(shz_vec2_sub(points[1], points[0]));

  pvr_vertex_t* vert = (pvr_vertex_t*)pvr_dr_target(state);
  vert->flags = PVR_CMD_VERTEX;
  shz_vec3_deref(&(vert->x)) = render_perspective_div(shz_xmtrx_transform_vec4(
      shz_vec4_init(points[0].x - v0.y * line_width * 0.5f,
                    points[0].y + v0.x * line_width * 0.5f,
                    (zvalues != NULL ? zvalues[0] : 0.0f), 1.0f)));
  vert->argb = color.raw;
  pvr_dr_commit(vert);

  vert = (pvr_vertex_t*)pvr_dr_target(state);
  vert->flags = PVR_CMD_VERTEX;
  shz_vec3_deref(&(vert->x)) = render_perspective_div(shz_xmtrx_transform_vec4(
      shz_vec4_init(points[0].x + v0.y * line_width * 0.5f,
                    points[0].y - v0.x * line_width * 0.5f,
                    (zvalues != NULL ? zvalues[0] : 0.0f), 1.0f)));
  vert->argb = color.raw;
  pvr_dr_commit(vert);

  for (int i = 0; i < point_count - 2; i++) {
    shz_vec2_t v1 =
        shz_vec2_normalize(shz_vec2_sub(points[i + 2], points[i + 1]));
    shz_vec2_t bisector =
        shz_vec2_scale(shz_vec2_normalize(shz_vec2_add(v0, v1)), line_width);

    vert = (pvr_vertex_t*)pvr_dr_target(state);
    shz_vec3_deref(&(vert->x)) =
        render_perspective_div(shz_xmtrx_transform_vec4(shz_vec4_init(
            points[i + 1].x - bisector.y, points[i + 1].y + bisector.x,
            (zvalues != NULL ? zvalues[i + 1] : 0.0f), 1.0f)));
    pvr_dr_commit(vert);

    vert = (pvr_vertex_t*)pvr_dr_target(state);
    shz_vec3_deref(&(vert->x)) =
        render_perspective_div(shz_xmtrx_transform_vec4(shz_vec4_init(
            points[i + 1].x + bisector.y, points[i + 1].y - bisector.x,
            (zvalues != NULL ? zvalues[i + 1] : 0.0f), 1.0f)));
    pvr_dr_commit(vert);
    v0 = v1;
  }

  vert = (pvr_vertex_t*)pvr_dr_target(state);
  shz_vec3_deref(&(vert->x)) = render_perspective_div(shz_xmtrx_transform_vec4(
      shz_vec4_init(points[point_count - 1].x - v0.y * line_width * 0.5f,
                    points[point_count - 1].y + v0.x * line_width * 0.5f,
                    (zvalues != NULL ? zvalues[point_count - 1] : 0.0f), 1.0f)));
  pvr_dr_commit(vert);

  vert = (pvr_vertex_t*)pvr_dr_target(state);
  vert->flags = PVR_CMD_VERTEX_EOL;
  shz_vec3_deref(&(vert->x)) = render_perspective_div(shz_xmtrx_transform_vec4(
      shz_vec4_init(points[point_count - 1].x + v0.y * line_width * 0.5f,
                    points[point_count - 1].y - v0.x * line_width * 0.5f,
                    (zvalues != NULL ? zvalues[point_count - 1] : 0.0f), 1.0f)));
  pvr_dr_commit(vert);

  pvr_dr_finish();
}
