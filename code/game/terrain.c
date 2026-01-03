
#include <dc/video.h>
#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/terrain.h>
#include <malloc.h>
#include <sh4zam/shz_sh4zam.h>
#include <stdlib.h>

static void subdivide(terrain_t* terrain, size_t offset_left,
                      size_t offset_right, float roughness) {
  shz_vec2_t* verts = terrain->verts;
  if (offset_left + 1 >= offset_right) {
    return;
  }
  size_t offset_mid = (offset_left + offset_right) >> 1;
  float displacement = ((float)rand() / (float)RAND_MAX - 0.5f) * roughness *
                       (float)(offset_right - offset_left) * 2.0f;

  verts[offset_mid].y =
      shz_lerpf(verts[offset_left].y, verts[offset_right].y, 0.5f) +
      displacement;
  if (verts[offset_mid].y < terrain->min_y) {
    verts[offset_mid].y = terrain->min_y;
  } else if (verts[offset_mid].y > terrain->max_y) {
    verts[offset_mid].y = terrain->max_y;
  }
  subdivide(terrain, offset_left, offset_mid, roughness * 0.75);
  subdivide(terrain, offset_mid, offset_right, roughness * 0.75);
}

#define PLAYER_TERRAIN_VERT_WIDTH 5

terrain_t* terrain_generate(int num_players, float roughness,
                            float prev_last_y) {
  size_t num_verts = vid_mode->width >> 2;  // one vertex every 4 pixels
  terrain_t* terrain =
      memalign(32, sizeof(terrain_t) + sizeof(shz_vec2_t) * num_verts);
  if (!terrain) {
    return NULL;
  }
  terrain->verts = (shz_vec2_t*)(terrain + 1);
  float x_step = (float)(vid_mode->width * ENJ_XSCALE) / (float)(num_verts - 1);
  for (size_t i = 0; i < num_verts; i++) {
    terrain->verts[i].x = x_step * (float)i;
  }

  terrain->min_y = (vid_mode->height >> 7);
  terrain->max_y = (vid_mode->height >> 4) * 11;
  terrain->roughness = roughness;

  terrain->num_verts = num_verts;
  terrain->verts[0].x = 0.0f;
  terrain->verts[0].y = prev_last_y;
  terrain->verts[num_verts - 1].y =
      terrain->min_y +
      (float)(rand() % (int)(terrain->max_y - terrain->min_y) >> 1);

  terrain->verts[num_verts - 1].x = (float)vid_mode->width * ENJ_XSCALE;
  subdivide(terrain, 0, terrain->num_verts - 1, roughness);

  // make small flat section for each player start
  int vert_seperation =
      (num_verts - num_players * PLAYER_TERRAIN_VERT_WIDTH) / (num_players + 1);
  for (int i = 1; i <= num_players; i++) {
    size_t platform_start_index =
        i * vert_seperation + (i - 1) * PLAYER_TERRAIN_VERT_WIDTH;
    float platform_height =
        terrain->verts[platform_start_index + (PLAYER_TERRAIN_VERT_WIDTH >> 1)]
            .y;

    for (int p = 0; p < PLAYER_TERRAIN_VERT_WIDTH; p++) {
      terrain->verts[platform_start_index + p].y = platform_height;
    }
    terrain->player_positions[i - 1] = (shz_vec2_t){
        .x =
            terrain
                ->verts[platform_start_index + (PLAYER_TERRAIN_VERT_WIDTH >> 1)]
                .x,
        .y = platform_height};
  }

  return terrain;
}
