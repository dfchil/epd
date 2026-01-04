#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/terrain.h>
#include <mortarlity/render/primitives.h>
#include <mortarlity/game/scene.h>

void _render_terrain_OP(void* data) {
  scene_t* a_scene = (scene_t*)data;
  terrain_t* terrain = a_scene->terrain;
  float zvalue = 1.0f;
  float line_width = 1.333f;

  float offset_x = (float)a_scene->offset_x * ENJ_XSCALE;
  render_strip_line(terrain->verts, terrain->num_verts, zvalue,
                     line_width, (enj_color_t){.raw = 0xFF007f00}, PVR_LIST_OP_POLY);
}

void render_terrain(scene_t* scene) {
  enj_render_list_add(PVR_LIST_OP_POLY, _render_terrain_OP, scene);
}
