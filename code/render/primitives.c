#include <enDjinn/enj_enDjinn.h>
#include <sh4zam/shz_sh4zam.h>

void render_sprite_line(shz_vec2_t from, shz_vec2_t to, float zvalue,
                        float line_width, pvr_dr_state_t *state_ptr) {
  shz_vec2_t direction = shz_vec2_normalize(shz_vec2_sub(to, from));

  float corners[4][3] = {
      {from.x, from.y, zvalue},
      {from.x + direction.y * line_width * ENJ_XSCALE,
       from.y - direction.x * line_width, zvalue},
      {to.x + direction.y * line_width * ENJ_XSCALE,
       to.y - direction.x * line_width, zvalue},
      {to.x, to.y, zvalue},
  };
  enj_draw_sprite(corners, state_ptr, NULL, NULL);
}

void render_strip_line(shz_vec2_t *points, int point_count, shz_vec3_t *offset,
                       float line_width, enj_color_t color, pvr_list_t list,
                       float *zvalues) {

  pvr_dr_state_t state;
  pvr_dr_init(&state);
  pvr_poly_cxt_t cxt;
  pvr_poly_cxt_col(&cxt, list);
  cxt.gen.culling = PVR_CULLING_NONE;

  pvr_poly_hdr_t *hdr = (pvr_poly_hdr_t *)pvr_dr_target(state);
  pvr_poly_compile(hdr, &cxt);
  pvr_dr_commit(hdr);

  shz_vec2_t v0 = shz_vec2_normalize(shz_vec2_sub(points[1], points[0]));

  pvr_vertex_t *vert = (pvr_vertex_t *)pvr_dr_target(state);
  vert->flags = PVR_CMD_VERTEX;
  vert->x = (offset->x + points[0].x - v0.y * line_width * 0.5f) * ENJ_XSCALE;
  vert->y =
      (offset->y + vid_mode->height - (points[0].y + v0.x * line_width * 0.5f));
  vert->z = offset->z + (zvalues != NULL ? zvalues[0] : 0.0f);
  vert->argb = color.raw;
  pvr_dr_commit(vert);

  vert = (pvr_vertex_t *)pvr_dr_target(state);
  vert->flags = PVR_CMD_VERTEX;
  vert->x = (offset->x + points[0].x + v0.y * line_width * 0.5f) * ENJ_XSCALE;
  vert->y =
      offset->y + vid_mode->height - (points[0].y - v0.x * line_width * 0.5f);
  vert->z = offset->z + (zvalues != NULL ? zvalues[0] : 0.0f);
  vert->argb = color.raw;
  pvr_dr_commit(vert);

  for (int i = 0; i < point_count - 2; i++) {
    shz_vec2_t v1 =
        shz_vec2_normalize(shz_vec2_sub(points[i + 2], points[i + 1]));
    shz_vec2_t bisector = shz_vec2_scale(
        shz_vec2_normalize(shz_vec2_add(v0, v1)), line_width * 0.75f);

    vert = (pvr_vertex_t *)pvr_dr_target(state);
    // vert->flags = PVR_CMD_VERTEX;
    vert->x = (offset->x + points[i + 1].x - bisector.y) * ENJ_XSCALE;
    vert->y = offset->y + vid_mode->height - (points[i + 1].y + bisector.x);
    vert->z = offset->z + (zvalues != NULL ? zvalues[i + 1] : 0.0f);
    // vert->argb = color.raw;
    pvr_dr_commit(vert);

    vert = (pvr_vertex_t *)pvr_dr_target(state);
    // vert->flags = PVR_CMD_VERTEX;
    vert->x = (offset->x + points[i + 1].x + bisector.y) * ENJ_XSCALE;
    vert->y = offset->y + vid_mode->height - (points[i + 1].y - bisector.x);
    vert->z = offset->z + (zvalues != NULL ? zvalues[i + 1] : 0.0f);
    // vert->argb = color.raw;
    pvr_dr_commit(vert);

    v0 = v1;
  }

  vert = (pvr_vertex_t *)pvr_dr_target(state);
  // vert->flags = PVR_CMD_VERTEX;
  vert->x = (offset->x + points[point_count - 1].x - v0.y * line_width * 0.5f) *
            ENJ_XSCALE;
  vert->y = (offset->y + vid_mode->height -
             (points[point_count - 1].y + v0.x * line_width * 0.5f));
  vert->z = offset->z + (zvalues != NULL ? zvalues[point_count - 1] : 0.0f);
  // vert->argb = color.raw;
  pvr_dr_commit(vert);

  vert = (pvr_vertex_t *)pvr_dr_target(state);
  vert->flags = PVR_CMD_VERTEX_EOL;
  vert->x = (offset->x + points[point_count - 1].x + v0.y * line_width * 0.5f) *
            ENJ_XSCALE;
  vert->y = (offset->y + vid_mode->height -
             (points[point_count - 1].y - v0.x * line_width * 0.5f));
  vert->z = offset->z + (zvalues != NULL ? zvalues[point_count - 1] : 0.0f);
  // vert->argb = color.raw;
  pvr_dr_commit(vert);

  pvr_dr_finish();
}
