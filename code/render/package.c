#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/render/package.h>
#include <mortarlity/render/primitives.h>

void _render_package_TR(void* data) {
  pkg_t* pkg = (pkg_t*)data;
  enj_color_t pkg_color = pkg->origin->color.primary;
  enj_color_t trail_color = pkg->origin->color.primary;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_TR_POLY);
  pvr_sprite_hdr_t hdr_s;
  pvr_sprite_compile(&hdr_s, &cxt);
  hdr_s.argb = pkg_color.raw;

  float offset_x = (float)((scene_t*)pkg->origin->scene)->offset_x;

#define PKG_SIZE 1.50f
#define PKG_Z 3.0f
  // if (pkg->moving) {
    enj_draw_sprite(
        (float[4][3]){
            {(pkg->position.x + offset_x - PKG_SIZE) * ENJ_XSCALE,
             vid_mode->height - pkg->position.y - PKG_SIZE, PKG_Z},
            {(pkg->position.x + offset_x + PKG_SIZE) * ENJ_XSCALE,
             vid_mode->height - pkg->position.y - PKG_SIZE, PKG_Z},
            {(pkg->position.x + offset_x + PKG_SIZE) * ENJ_XSCALE,
             vid_mode->height - pkg->position.y + PKG_SIZE, PKG_Z},
            {(pkg->position.x + offset_x - PKG_SIZE) * ENJ_XSCALE,
             vid_mode->height - pkg->position.y + PKG_SIZE, PKG_Z},
        },
        &state, &hdr_s, NULL);
  // }
  pvr_poly_cxt_t cxt_poly;
  pvr_poly_cxt_col(&cxt_poly, PVR_LIST_TR_POLY);
  cxt_poly.gen.culling = PVR_CULLING_NONE;
  pvr_poly_hdr_t* hdr_pp = (pvr_poly_hdr_t*)pvr_dr_target(state);
  pvr_poly_compile(hdr_pp, &cxt_poly);
  pvr_dr_commit(hdr_pp);

#define PKG_TRAIL_WIDTH 3.0f

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

    shz_vec2_t* from = pkg->trail + sprite_pos;
    shz_vec2_t* to =
        pkg->trail + ((sprite_pos + 1) % PACKAGE_TRAIL_STEPS);

    shz_vec2_t dir = shz_vec2_normalize(shz_vec2_sub(*to, *from));

    pvr_vertex_t* vert = (pvr_vertex_t*)pvr_dr_target(state);
    vert->flags = PVR_CMD_VERTEX;
    vert->x = (from->x + offset_x + dir.y * PKG_TRAIL_WIDTH) * ENJ_XSCALE;
    vert->y = vid_mode->height - (from->y - dir.x * PKG_TRAIL_WIDTH);
    vert->z = 1.0f;
    vert->argb = trail_color.raw;
    pvr_dr_commit(vert);

    vert = (pvr_vertex_t*)pvr_dr_target(state);
    vert->flags = last_vert_pair ? PVR_CMD_VERTEX_EOL : PVR_CMD_VERTEX;
    vert->x = (from->x + offset_x - dir.y * PKG_TRAIL_WIDTH) * ENJ_XSCALE;
    vert->y = vid_mode->height - (from->y + dir.x * PKG_TRAIL_WIDTH);
    vert->z = 1.0f;
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
  // render_strip_line(terrain->verts + pkg->shadow_min_x, pkg->shadow_max_x - pkg->shadow_min_x + 1,
  //                   &(shz_vec3_t){.x = - 0.5f, .y = 0.0f, .z = 22.1f}, 2.333f,
  //                   shadow_color, PVR_LIST_TR_POLY, NULL);
}

void render_pkg(pkg_t* pkg) {
  enj_render_list_add(PVR_LIST_TR_POLY, _render_package_TR, (void*)pkg);
}