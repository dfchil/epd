#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/game/scene.h>
#include <mortarlity/modes/title_screen.h>

static enj_mode_t gameplay = {
    .name = "gameplay mode",
    .on_activation_fn = NULL,
    .mode_updater = scene_updater,
    .data = NULL,
};

void title_screen_updater(__unused void *data) {
  gameplay.data = scene_construct(8, NULL);
  enj_mode_set(&gameplay);
}