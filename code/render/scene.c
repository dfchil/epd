#include <mortarlity/render/scene.h> 
#include <mortarlity/render/terrain.h>
#include <mortarlity/render/player.h>

void render_scene(scene_t *scene) {
  if (scene->terrain) {
    render_terrain(scene);
  }
  for (int i = 0; i < scene->num_players; i++) {
    render_player(scene->players + i);
  }
}