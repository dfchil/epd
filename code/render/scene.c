#include <gorgol8/render/scene.h> 
#include <gorgol8/render/terrain.h>
#include <gorgol8/render/player.h>

void render_scene(scene_t *scene) {
  if (scene->terrain) {
    render_terrain(scene);
    enj_render_list_add(PVR_LIST_PT_POLY, render_terrain_stats,
                        scene);
  }
  for (int i = 0; i < scene->num_players; i++) {
    render_player(scene->players + i);
  }
}