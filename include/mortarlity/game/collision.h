#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <mortarlity/game/player.h>
#include <mortarlity/game/shell.h>
#include <mortarlity/game/terrain.h>


/** Line-line collision detection
 * @param l1_start Start point of line 1
 * @param l1_end End point of line 1
 * @param l2_start Start point of line 2
 * @param l2_end End point of line 2
 * @return The fractional distance along line 1 where the collision occurs, or -1.0f if no collision
 */
float collision_line_line(shz_vec2_t* l1_start, shz_vec2_t* l1_end,
                          shz_vec2_t* l2_start, shz_vec2_t* l2_end);

#endif // GAME_COLLISION_H