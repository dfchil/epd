#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <mortarlity/game/player.h>
#include <mortarlity/game/package.h>
#include <mortarlity/game/terrain.h>


/** Line-line collision detection
 * @param l1_start Start point of line 1
 * @param l1_vec Vector of line 1
 * @param l2_start Start point of line 2
 * @param l2_vec Vector of line 2
 * @return The fractional distance along line 1 where the collision occurs, or -1.0f if no collision
 */
float collision_line_line(shz_vec2_t* l1_start, shz_vec2_t* l1_vec,
                          shz_vec2_t* l2_start, shz_vec2_t* l2_vec);

/** Line-player collision detection
 * @param player The player to test against
 * @param line_start Start point of the line
 * @param line_vec Vector of the line
 *  @return The fractional distance along the line where the collision occurs, or -1.0f if no collision
 */
float collision_player_line(game_player_t *player, shz_vec2_t *line_start, shz_vec2_t *line_vec);
#endif // GAME_COLLISION_H