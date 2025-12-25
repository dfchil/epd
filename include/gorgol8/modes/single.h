#ifndef MODE_SINGLE_H
#define MODE_SINGLE_H

#include <enDjinn/enj_enDjinn.h>
#include <gorgol8/game/terrain.h>

typedef struct single_mode_s {
  terrain_t* terrain;
} single_mode_t;


void mode_single_regen(void);
enj_mode_t* mode_single_get(void); 


#endif // MODE_SINGLE_H