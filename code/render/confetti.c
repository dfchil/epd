#include <mortarlity/render/confetti.h>

void _render_confett_TR(void* data) {
  confetti_cluster_t* cluster = (confetti_cluster_t*)data;

  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_TR_POLY);
  cxt.gen.culling = PVR_CULLING_NONE;
  pvr_sprite_hdr_t hdr_s;
  pvr_sprite_compile(&hdr_s, &cxt);

  float life_ratio =
      1.0f - (cluster->age / (float)cluster->lifetime);  // 1.0 to 0.0

  uint8_t alpha = (uint8_t)(255 * life_ratio);
  cluster->color_src.a = alpha;
  cluster->color_dst.a = alpha;

  for (int i = 0; i < cluster->num_particles; i++) {
    hdr_s.argb = i & 1 == 1 ? cluster->color_src.raw : cluster->color_dst.raw;

    confetti_particle_t* particle = cluster->particles + i;
    float size = particle->size * 2.0f;

    float corners[4][3];
    *(shz_vec3_t*)(corners[0]) = shz_quat_transform_vec3(
        particle->rotation, (shz_vec3_t){.x = -size, .y = -size, .z = 0.0f});
    *(shz_vec3_t*)(corners[1]) = shz_quat_transform_vec3(
        particle->rotation, (shz_vec3_t){.x = size, .y = -size, .z = 0.0f});
    *(shz_vec3_t*)(corners[2]) = shz_quat_transform_vec3(
        particle->rotation, (shz_vec3_t){.x = size, .y = size, .z = 0.0f});
    *(shz_vec3_t*)(corners[3]) = shz_quat_transform_vec3(
        particle->rotation, (shz_vec3_t){.x = -size, .y = size, .z = 0.0f});

    for (int c = 0; c < 4; c++) {
      corners[c][0] = (particle->position.x + corners[c][0]) * ENJ_XSCALE;
      corners[c][1] = vid_mode->height - (particle->position.y + corners[c][1]);
      corners[c][2] = 2.0f;
    }

    enj_draw_sprite(corners, &state, &hdr_s, NULL);
  }
  pvr_dr_finish();
}

void render_confetti(confetti_cluster_t* cluster) {
  enj_render_list_add(PVR_LIST_TR_POLY, _render_confett_TR, (void*)cluster);
}
