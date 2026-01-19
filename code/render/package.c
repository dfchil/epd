#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/package.h>
#include <mortarlity/render/primitives.h>

#define PKG_HALF_SIZE 1.5f
#define PKG_Z 3.0f

// clang-format off
alignas(32) static const shz_vec4_t pgk_mdl[4] = {
    {-PKG_HALF_SIZE, -PKG_HALF_SIZE, 0.0f, 1.0f},
    { PKG_HALF_SIZE, -PKG_HALF_SIZE, 0.0f, 1.0f},
    { PKG_HALF_SIZE,  PKG_HALF_SIZE, 0.0f, 1.0f},
    {-PKG_HALF_SIZE,  PKG_HALF_SIZE, 0.0f, 1.0f},
};
// clang-format on

void _render_package_TR(void *data) {
  pkg_t *pkg = (pkg_t *)data;
  enj_color_t pkg_color = pkg->origin->color.primary;
  enj_color_t trail_color = pkg->origin->color.primary;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_TR_POLY);
  cxt.gen.culling = PVR_CULLING_CW;
  pvr_sprite_hdr_t hdr_s;
  pvr_sprite_compile(&hdr_s, &cxt);
  hdr_s.argb = pkg_color.raw;

  shz_xmtrx_init_translation(
      ((float)((scene_t *)pkg->origin->scene)->offset_x + pkg->position.x) *
          ENJ_XSCALE,
      vid_mode->height - pkg->position.y, PKG_Z);
  shz_xmtrx_apply_scale(ENJ_XSCALE, -1.0f, 1.0f);
  float delta_angle = pkg->velocity.x != 0.0f || pkg->velocity.y != 0.0f
                          ? shz_atan2f(pkg->velocity.y, pkg->velocity.x)
                          : 0.0f;

  alignas(32) float pgk_corners[4][3];
  for (int c = 0; c < 4; c++) {
    shz_vec3_deref(pgk_corners[c]) =
        render_perspective_div(shz_xmtrx_transform_vec4(pgk_mdl[c]));
  }
  enj_draw_sprite(pgk_corners, &state, &hdr_s, NULL);

  pvr_poly_cxt_t cxt_poly;
  pvr_poly_cxt_col(&cxt_poly, PVR_LIST_TR_POLY);
  cxt_poly.gen.culling = PVR_CULLING_NONE;
  pvr_poly_hdr_t *hdr_pp = (pvr_poly_hdr_t *)pvr_dr_target(state);
  pvr_poly_compile(hdr_pp, &cxt_poly);
  pvr_dr_commit(hdr_pp);

#define PKG_TRAIL_WIDTH 3.0f

  float offset_x =
      (float)((scene_t *)pkg->origin->scene)->offset_x * ENJ_XSCALE;

  shz_xmtrx_set_translation(offset_x, vid_mode->height, PKG_Z - 0.1f);
  for (int i = 0; i < pkg->used_trail_steps; i++) {
    int last_vert_pair = i == pkg->used_trail_steps - 1;
    int sprite_pos = package_modulo(-pkg->frame + i, PACKAGE_TRAIL_STEPS);
    if (pkg->moving) {
      trail_color.a = PACKAGE_TRAIL_STEPS - i;
    } else {
      trail_color.a = (uint8_t)(pkg->trail_fade - i);
    }
    if (trail_color.a <= 0) {
      trail_color.a = 0;
      last_vert_pair = 1;
    }
    shz_vec2_t *from = pkg->trail + sprite_pos;
    shz_vec2_t *to =
        pkg->trail + package_modulo(sprite_pos + 1, PACKAGE_TRAIL_STEPS);

    shz_vec2_t dir = shz_vec2_normalize(shz_vec2_sub(*to, *from));

    pvr_vertex_t *vert = (pvr_vertex_t *)pvr_dr_target(state);

    vert->flags = PVR_CMD_VERTEX;
    shz_vec3_deref(&(vert->x)) =
        render_perspective_div(shz_xmtrx_transform_vec4(
            shz_vec4_init((from->x + dir.y * PKG_TRAIL_WIDTH),
                          (from->y - dir.x * PKG_TRAIL_WIDTH), 1.0f, 1.0f)));
    vert->argb = trail_color.raw;
    pvr_dr_commit(vert);

    vert = (pvr_vertex_t *)pvr_dr_target(state);
    vert->flags = last_vert_pair ? PVR_CMD_VERTEX_EOL : PVR_CMD_VERTEX;
    shz_vec3_deref(&(vert->x)) =
        render_perspective_div(shz_xmtrx_transform_vec4(
            shz_vec4_init((from->x - dir.y * PKG_TRAIL_WIDTH),
                          (from->y + dir.x * PKG_TRAIL_WIDTH), 1.0f, 1.0f)));
    vert->argb = trail_color.raw;
    pvr_dr_commit(vert);
    if (last_vert_pair) {
      break;
    }
  }
  pvr_dr_finish();

  // enj_color_t shadow_color = pkg->origin->color.primary;
  // shadow_color.a = 255;

  // terrain_t* terrain = (terrain_t*)((scene_t*)pkg->origin->scene)->terrain;
  // render_strip_line(terrain->verts + pkg->shadow_min_x, pkg->shadow_max_x -
  // pkg->shadow_min_x + 1,
  //                   &(shz_vec3_t){.x = - 0.5f, .y = 0.0f, .z
  //                   = 22.1f}, 2.333f, shadow_color, PVR_LIST_TR_POLY, NULL);
}

void render_pkg(pkg_t *pkg) {
  enj_render_list_add(PVR_LIST_TR_POLY, _render_package_TR, (void *)pkg);
}