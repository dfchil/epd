#include <dc/sound/sfxmgr.h>
#include <dc/sound/sound.h>
#include <dc/video.h>
#include <enDjinn/enj_enDjinn.h>
#include <mortarlity/sfx/sounds.h>
#include <sys/param.h>

// clang-format off
static const char *filenames[] = {
    "launch.dca", 
    "thudd.dca",
    "children_yay.dca",
    "yay_subdued.dca",
    "sarcastic.dca",
};
// clang-format on

static sfxhnd_t _sfxs[SFX_COUNT];

static void _sfx_load(const char *filename, sfx_ref_t sfx) {
  char filepath[256];
  ENJ_DEBUG_PRINT("Loading SFX: %s\n", filename);
  snprintf(filepath, sizeof(filepath) - 8, ENJ_CBASEPATH "/sfx/PCM/16/%s",
           filename);
  _sfxs[sfx] = enj_sound_dca_load_file(filepath);
  if (_sfxs[sfx] == SFXHND_INVALID) {
    ENJ_DEBUG_PRINT("Failed to load SFX: %s\n", filepath);
  }
}

void sfx_init() {
  for (int i = 0; i < SFX_COUNT; i++) {
    _sfx_load(filenames[i], (sfx_ref_t)i);
  }
}

int sfx_play(sfx_ref_t sfx, int volume, int pan) {
  if (_sfxs[sfx] == SFXHND_INVALID) {
    return -1;
  }
  return snd_sfx_play(_sfxs[sfx], MIN(volume, 255), pan);
}

int sfx_pos2pan(float xpos) {
  float pan_ratio = (xpos / (float)vid_mode->width);
  return (int)(pan_ratio * 255.0f);
}