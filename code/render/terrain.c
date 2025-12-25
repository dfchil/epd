#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/game/terrain.h>
#include <gorgol8/render/primitives.h>

void render_terrain_stats(void* data) {
  terrain_t* terrain = (terrain_t*)data;

  enj_qfont_color_set(0xff, 0, 0);  // red color

  int xpos = 10;
  int ypos = 10;

  char buffer[128];

  snprintf(buffer, sizeof(buffer), "Terrain Vertices: %zu", terrain->num_verts);
  ypos += enj_qfont_get_header()->line_height;
  enj_qfont_write(buffer, xpos, ypos, PVR_LIST_PT_POLY);
  ypos += enj_qfont_get_header()->line_height;
  snprintf(buffer, sizeof(buffer), "Min Y: %.2f", terrain->min_y);
  ypos += enj_qfont_get_header()->line_height;
  enj_qfont_write(buffer, xpos, ypos, PVR_LIST_PT_POLY);
  snprintf(buffer, sizeof(buffer), "Max Y: %.2f", terrain->max_y);
  ypos += enj_qfont_get_header()->line_height;
  enj_qfont_write(buffer, xpos, ypos, PVR_LIST_PT_POLY);
  snprintf(buffer, sizeof(buffer), "Seed: %d", terrain->seed);
  ypos += enj_qfont_get_header()->line_height;
  enj_qfont_write(buffer, xpos, ypos, PVR_LIST_PT_POLY);
  snprintf(buffer, sizeof(buffer), "Roughness: %.2f", terrain->roughness);
  ypos += enj_qfont_get_header()->line_height;
  enj_qfont_write(buffer, xpos, ypos, PVR_LIST_PT_POLY);
}

void render_terrain(void* data) {
  terrain_t* terrain = (terrain_t*)data;
  float zvalue = 1.0f;
  float line_width = 1.0f;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_OP_POLY);
  cxt.gen.culling = PVR_CULLING_NONE;
  pvr_sprite_hdr_t* hdr = (pvr_sprite_hdr_t*)pvr_dr_target(
      state);  // skip ahead to commit header first
  pvr_sprite_compile(hdr, &cxt);
  hdr->argb = 0xFF00ff00;  // green color

  pvr_dr_commit(hdr);
  
  for (size_t i = 0; i < terrain->num_verts - 1; i++) {
    // pvr_sprite_hdr_t* hdr = (pvr_sprite_hdr_t*)pvr_dr_target(
    //     state);  // skip ahead to commit header first
    // pvr_sprite_compile(hdr, &cxt);
    // hdr->argb = i & 1 ? 0xFF00ff00 : 0xFF0000ff;  // green or blue color
    // pvr_dr_commit(hdr);
    
    render_sprite_line(
        (shz_vec2_t){.x = terrain->verts[i].x,
                     .y = vid_mode->height - terrain->verts[i].y},
        (shz_vec2_t){.x = terrain->verts[i + 1].x,
                     .y = vid_mode->height - terrain->verts[i + 1].y},
        zvalue, line_width, &state);
  }
  pvr_dr_finish();
}