#include <stdint.h>

typedef enum : uint8_t {
  SFX_LAUNCH = 0,
  SFX_THUD,
  SFX_YAY_ENTHUSIASTIC,
  SFX_YAY_SUBDUED,
  SFX_SARCASTIC,
  SFX_COUNT,
} sfx_ref_t;

void sfx_init();
int sfx_play(sfx_ref_t sfx, int volume, int pan);
int sfx_pos2pan(float xpos);