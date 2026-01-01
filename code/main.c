#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/modes/scene.h>

int main(__unused int argc, __unused char **argv) {
  enj_state_init_defaults();
  enj_state_soft_reset_set(ENJ_BUTTON_DOWN << (8 << 1));
  enj_state_startup();

  scene_t* scene = scene_construct(8);
  enj_mode_push(scene_mode_get() );

  enj_state_run();
  return 0;
}
