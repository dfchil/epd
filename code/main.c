#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/modes/title_screen.h>
#include <mortarlity/game/player.h>
#include <mortarlity/sfx/sounds.h>

int main(__unused int argc, __unused char** argv) {

  player_setup_colors();

  enj_state_init_defaults();
  enj_state_soft_reset_set((ENJ_BUTTON_DOWN << (8 << 1)) | ENJ_BUTTON_DOWN);
  enj_state_startup();
  sfx_init();


  enj_mode_t titlescreen = {
      .name = "Title Screen",
      .on_activation_fn = NULL,
      .mode_updater = title_screen_updater,
      .data = NULL,
      .no_soft_reset = 0,
  };

  enj_mode_push(&titlescreen);
  enj_state_run();
  return 0;
}
