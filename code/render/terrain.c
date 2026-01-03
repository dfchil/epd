#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/game/terrain.h>
#include <gorgol8/render/primitives.h>
#include <gorgol8/game/scene.h>

void _render_terrain_OP(void* data) {
  scene_t* a_scene = (scene_t*)data;
  terrain_t* terrain = a_scene->terrain;
  float zvalue = 1.0f;
  float line_width = 1.0f;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_OP_POLY);
  pvr_sprite_hdr_t* hdr = (pvr_sprite_hdr_t*)pvr_dr_target(state);
  pvr_sprite_compile(hdr, &cxt);
  hdr->argb = 0xFF00ff00;  // green color

  pvr_dr_commit(hdr);

  float offset_x = (float)a_scene->offset_x * ENJ_XSCALE;
  
  for (size_t i = 0; i < terrain->num_verts - 1; i++) {
    // pvr_sprite_hdr_t* hdr = (pvr_sprite_hdr_t*)pvr_dr_target(
    //     state);  // skip ahead to commit header first
    // pvr_sprite_compile(hdr, &cxt);
    // hdr->argb = i & 1 ? 0xFF00ff00 : 0xFF0000ff;  // green or blue color
    // pvr_dr_commit(hdr);
    
    render_sprite_line(
        (shz_vec2_t){.x = terrain->verts[i].x + offset_x,
                     .y = vid_mode->height - terrain->verts[i].y},
        (shz_vec2_t){.x = terrain->verts[i + 1].x + offset_x,
                     .y = vid_mode->height - terrain->verts[i + 1].y},
        zvalue, line_width, &state);
  }
  pvr_dr_finish();
}

void render_terrain(scene_t* scene) {
  enj_render_list_add(PVR_LIST_OP_POLY, _render_terrain_OP, scene);
}
