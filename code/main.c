#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/game/scene.h>

int main(__unused int argc, __unused char** argv) {
  enj_state_init_defaults();
  enj_state_soft_reset_set((ENJ_BUTTON_DOWN << (8 << 1)) | ENJ_BUTTON_DOWN);
  enj_state_startup();

  enj_mode_t gameplay = {
      .name = "gameplay mode",
      .on_activation_fn = NULL,
      .mode_updater = scene_updater,
      .data = scene_construct(8, NULL),
  };
  enj_mode_push(&gameplay);
  enj_state_run();
  return 0;
}
