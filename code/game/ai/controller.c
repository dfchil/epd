#include <mortarlity/game/ai/controller.h>


/**
 * personalities for AI controllers
 * 
 * - nearest: targets nearest player, left to right
 * - furthest: targets furthest player, right to left
 * - vengeful: targets players who last hit it
 * - opportunistic: targets players with lowest health, starting with nearest
 * - usurper: targets players with highest health, furthest first
 * - egalitarian: selects targets by scoreboard rankings, highest to lowest
 * - bully: selects targets by scoreboard rankings, lowest to highest
 * - max-power: chooses next target at random, but uses max power shots only correcting angle
 * - chameleon: changes target selection strategy when current target is destroyed
 */
