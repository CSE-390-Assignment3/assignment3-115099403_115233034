#pragma once

#include "types.h"

#include <cstdlib>
#include <utility>
#include <vector>

// config
constexpr Pos DOCK_POS = {0, 0};
constexpr int BATTERY_BUFF = 2; // can be percentage
// config end

/**
 * @brief get std::pair corresponding to step direction
 *
 * @param dir
 * @param invert
 * @return Pos
 */
inline Pos toPair(Direction dir, bool invert = false);

/**
 * @brief get direction std::pair corresponding to direction
 *
 * @param dir
 * @param invert
 * @return Pos
 */
inline Direction toDir(int x, int y);

/**
 * @brief Get the Position in the given direction
 *
 * @param pos current position
 * @param dir direction
 * @return Pos
 */
Pos getPosition(Pos pos, Direction dir);

/**
 * @brief Prioirty of direction traversal
 *
 * @param dir
 * @return Direction
 */
Direction nextDir(const Direction &dir);

/**
 * @brief Prioirty of direction traversal
 */
std::vector<Direction> dirPriority();

Direction getDirection(Pos src, Pos dst);

std::ostream &operator<<(std::ostream &out, const AlgoState &step);
