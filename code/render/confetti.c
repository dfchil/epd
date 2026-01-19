#include <mortarlity/render/confetti.h>
#include <mortarlity/render/primitives.h>

#define C_HALF_SIZE 2.0f
alignas(32) static const shz_vec4_t corner_inputs[4] = {
    {.x = -C_HALF_SIZE, .y = -C_HALF_SIZE, .z = 1.0f, .w = 1.0f},
    {.x = -C_HALF_SIZE, .y = +C_HALF_SIZE, .z = 1.0f, .w = 1.0f},
    {.x = +C_HALF_SIZE, .y = +C_HALF_SIZE, .z = 1.0f, .w = 1.0f},
    {.x = +C_HALF_SIZE, .y = -C_HALF_SIZE, .z = 1.0f, .w = 1.0f},
};

void _render_confetti_TR(void *data) {
  confetti_cluster_t *cluster = (confetti_cluster_t *)data;
  pvr_dr_state_t state;
  pvr_dr_init(&state);

  pvr_sprite_cxt_t cxt;
  pvr_sprite_cxt_col(&cxt, PVR_LIST_TR_POLY);
  cxt.gen.culling = PVR_CULLING_NONE;
  pvr_sprite_hdr_t hdr_s;
  pvr_sprite_compile(&hdr_s, &cxt);

  float lifetime = cluster->age / (float)cluster->lifetime;
  cluster->color_dst.a = cluster->color_src.a =
      (uint8_t)(255 * (1.0f - lifetime));

  alignas(32) float corners[4][3];
  for (int i = 0; i < cluster->num_particles; i++) {
    confetti_particle_t *particle = cluster->particles + i;
    const float angle =
        lifetime * 6.28318f * particle->rotation_speed + particle->start_angle;
    shz_quat_t rotation_quat =
        shz_quat_from_axis_angle(particle->rotation, angle);
    shz_xmtrx_init_translation(particle->position.x * ENJ_XSCALE,
                               vid_mode->height - particle->position.y, 5.0f);
    shz_xmtrx_apply_scale(ENJ_XSCALE, 1.0f, 1.0f);
    shz_xmtrx_apply_rotation_quat(rotation_quat);

    for (int c = 0; c < 4; c++) {
      shz_vec3_deref(corners[c]) =
          render_perspective_div(shz_xmtrx_transform_vec4(corner_inputs[c]));
    }
    shz_vec3_t v0 =
        shz_vec3_sub(shz_vec3_deref(corners[1]), shz_vec3_deref(corners[0]));
    shz_vec3_t v1 =
        shz_vec3_sub(shz_vec3_deref(corners[2]), shz_vec3_deref(corners[0]));

    hdr_s.argb = shz_vec3_cross(v0, v1).z < 0.0f ? cluster->color_dst.raw
                                                 : cluster->color_src.raw;
    enj_draw_sprite(corners, &state, &hdr_s, NULL);
  }
  pvr_dr_finish();
}

void render_confetti(confetti_cluster_t *cluster) {
  enj_render_list_add(PVR_LIST_TR_POLY, _render_confetti_TR, (void *)cluster);
}
