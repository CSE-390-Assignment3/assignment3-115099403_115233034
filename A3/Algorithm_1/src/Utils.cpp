
#include "../include/Utils.h"

inline Pos toPair(Direction dir, bool invert) {
  int multiplier = invert ? -1 : 1;
  switch (dir) {
  case Direction::North:
    return {-1 * multiplier, 0};
  case Direction::South:
    return {1 * multiplier, 0};
  case Direction::East:
    return {0, 1 * multiplier};
  case Direction::West:
    return {0, -1 * multiplier};
  default:
    return {0, 0};
  }
}

inline Direction toDir(int x, int y) {
  /**
   * @todo handle errors for x, y
   */
  if (x == 1)
    return Direction::South;
  if (x == -1)
    return Direction::North;
  if (y == 1)
    return Direction::East;
  if (y == -1)
    return Direction::West;
  // default error value
  return Direction::North;
}

Pos getPosition(Pos pos, Direction dir) {
  auto direction = toPair(dir);
  return {pos.first + direction.first, pos.second + direction.second};
}

Direction getDirection(Pos src, Pos dst) {
  Pos diff = {dst.first - src.first, dst.second - src.second};
  if ((std::abs(diff.first) + std::abs(diff.second)) != 1) {
    std::cout << __FUNCTION__ << "ERROR!! invalid parameters in getDirection"
              << std::endl;
  }
  return toDir(diff.first, diff.second);
}

std::vector<Direction> dirPriority() {
  /**
   * @note
   * UP
   * DOWN
   * EAST
   * WEST
   */
  return {Direction::North, Direction::South, Direction::East, Direction::West};
}

Direction nextDir(const Direction &dir) {
  auto dl = dirPriority();
  int i = 0;
  while (dl[i] != dir)
    i++;
  return dl[(i + 1) % 4];
}

std::ostream &operator<<(std::ostream &out, const AlgoState &state) {
  switch (state) {
  case AlgoState::CHARGING:
    out << "CHARGING";
    break;
  case AlgoState::TO_DOCK:
    out << "TO_DOCK";
    break;
  case AlgoState::TO_POS:
    out << "TO_POS";
    break;
  case AlgoState::WORKING:
    out << "WORKING";
    break;
  case AlgoState::FINISH:
    out << "FINISH";
    break;
  case AlgoState::EXPLORE:
    out << "EXPLORE";
    break;
  case AlgoState::CLEANING:
    out << "CLEANING";
    break;
  default:
    out << "State";
  }
  return out;
}
