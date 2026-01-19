#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/game/terrain.h>
#include <mortarlity/render/primitives.h>

void _render_terrain_OP(void* data) {
  scene_t* a_scene = (scene_t*)data;
  terrain_t* terrain = a_scene->terrain;
  float offset_x = (float)a_scene->offset_x - 0.5f;

  render_strip_line(terrain->verts, terrain->num_verts + 1,
                    &(shz_vec3_t){.x = offset_x, .y = +vid_mode->height, .z = 0.1f}, 1.333f,
                    (enj_color_t){.raw = 0xFF007f00}, PVR_LIST_OP_POLY, NULL);
}

void render_terrain(scene_t* scene) {
  enj_render_list_add(PVR_LIST_OP_POLY, _render_terrain_OP, scene);
}
