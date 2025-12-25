#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/modes/single.h>

int main(__unused int argc, __unused char **argv) {
  enj_state_init_defaults();
  printf("supersampling: %d, %d\n", ENJ_SUPERSAMPLING, enj_state_get()->video.pvr_params.fsaa_enabled);

  enj_state_soft_reset_set(ENJ_BUTTON_DOWN << (8 << 1));

  enj_state_startup();

  mode_single_regen();
  enj_mode_push(mode_single_get());

  enj_state_run();
  return 0;
}
