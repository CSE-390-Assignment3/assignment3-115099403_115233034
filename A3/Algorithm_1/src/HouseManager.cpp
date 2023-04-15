#include "../include/HouseManager.h"

#include "../include/Utils.h"

#include <queue>
#include <vector>

HouseManager::HouseManager() : total_dirt_(0) {}

bool HouseManager::exists(const Pos pos) const {
  return percieved_house_.count(pos) != 0;
}

int HouseManager::dirt(const Pos pos) {
  if (exists(pos))
    return percieved_house_[pos];
  std::cout << "ERROR!! " << __FUNCTION__ << " position does not exist. "
            << std::endl;
  return -2; // @todo errorcodes.h for algo
}

void HouseManager::setDirt(const Pos pos, int dirtlevel) {
  if (/* percieved_house_.count(pos) != 0 && */ percieved_house_[pos] > 0 &&
      percieved_house_[pos] <= MAX_DIRT)
    total_dirt_ -= percieved_house_[pos];

  percieved_house_[pos] = dirtlevel;
  total_dirt_ += (dirtlevel >= 0 && dirtlevel <= MAX_DIRT) ? dirtlevel : 0;
}

bool HouseManager::isWall(const Pos pos) {
  return percieved_house_[pos] == int(LocType::Wall);
}

bool HouseManager::isDock(const Pos pos) {
  return percieved_house_[pos] == int(LocType::Dock);
}

void HouseManager::clean(const Pos pos) {
  if (percieved_house_[pos] > 0 && percieved_house_[pos] <= MAX_DIRT) {
    // clean pos
    percieved_house_[pos]--;
    total_dirt_--;
  }
}

bool HouseManager::isUnexploredEmpty() {
  // std::cout << " " << unexplored_points_.size() << " ";
  return unexplored_points_.empty();
}

bool HouseManager::isUnexplored(const Pos pos) {
  return unexplored_points_.count(pos) != 0;
}

void HouseManager::eraseUnexplored(const Pos pos) {
  if (isUnexplored(pos))
    unexplored_points_.erase(pos);
}

void HouseManager::updateNeighbor(Direction dir, Pos position, bool isWall) {
  auto pos = getPosition(position, dir);
  if (isWall) {
    percieved_house_[pos] = int(LocType::Wall);
  } else {
    if (percieved_house_.count(pos) == 0)
      unexplored_points_[pos];
  }
}

/**
 * @brief update dirt at pos and clean
 *
 * @param pos position where value needs to be updated
 * @param dirt value to be updated
 */
void HouseManager::clean(const Pos pos, int dirt) {
  if (/* percieved_house_.count(pos) != 0 && */ percieved_house_[pos] > 0 &&
      percieved_house_[pos] <= MAX_DIRT)
    total_dirt_ -= percieved_house_[pos];

  percieved_house_[pos] = dirt;
  total_dirt_ += (dirt >= 0 && dirt <= MAX_DIRT) ? dirt : 0;

  clean(pos);
}

/**
 * @brief Returns stack of directions to take to reach given destination if
 * search is false and if search is true returns path to closest dirt or
 * unexplored location
 *
 * @param src Starting location
 * @param dst
 * @param search if true returns closest dirty/unexplored
 * @return std::stack<Direction>
 */
std::stack<Direction> HouseManager::getShortestPath(std::pair<int, int> src,
                                                    std::pair<int, int> dst,
                                                    bool search) {
  std::stack<Direction> path;

  std::queue<Pos> q; // stack for DFS traversal
  std::map<Pos, bool> visited;
  std::map<Pos, Pos> parent;

  q.push(src);
  visited[src] = true;

  bool found = false;

  // std::cout << __FUNCTION__ << " dst " << dst.first << "," << dst.second
  //           << std::endl;

  /**
   * @todo
   * - multiple paths till some battery level
   * - Priority queue of paths (with total dirt, distance)
   * - Store all paths and choose best based on difference battery left and
   *   min path coupled with dirt cleaned on way back
   *
   *  ** for A2 we can implement best path on given depth
   */
  while (!q.empty()) {
    auto t = q.front();
    // std::cout << __FUNCTION__ << "top " << t.first << "," << t.second
    //           << std::endl;
    q.pop();
    for (std::pair<int, int> v : neighbors(t)) {
      if (visited.count(v) == 0) { // !visited
        q.push(v);
        visited[v] = true;
        parent[v] = t;
      }
    }
    if (search) {
      if (!((percieved_house_.count(t) != 0 && percieved_house_[t] > 0) ||
            unexplored_points_.count(t) != 0)) { // found dirt
        continue;
      }
    }
    if (!search) {
      if ((!path.empty() || t != dst))
        continue; // if path is already found or not target node
    }
    // std::cout << __FUNCTION__ << " FOUND PATH! " << std::endl;
    auto v = t;
    while (v != src) {
      // path.push(v);
      path.push(getDirection(parent[v], v));
      v = parent[v];
    }
    break;
  }
  return path;
}

std::vector<std::pair<int, int>>
HouseManager::neighbors(std::pair<int, int> point) {
  static std::vector<std::pair<int, int>> directions = {
      {-1, 0}, {1, 0}, {0, 1}, {0, -1}};
  std::vector<std::pair<int, int>> neighbors;

  for (auto dir : directions) {
    std::pair<int, int> temp = {point.first + dir.first,
                                point.second + dir.second};
    // do not add walls
    // do not add unvisited nodes
    // std::cout << "temp: " << temp.first << "," << temp.second << std::endl;
    // std::cout << "values: " << percieved_house_.count(temp) << "  "
    //           << isWall(temp) << std::endl;
    if ((percieved_house_.count(temp) != 0 &&
         !isWall(temp) /*  && !isDock(temp) */) ||
        unexplored_points_.count(temp) != 0) {
      neighbors.push_back(temp);
    }
  }
  // std::cout << __FUNCTION__ << " NEIGHBORS PRINT : " << neighbors.size()
  //           << std::endl;
  // for (auto n : neighbors) {
  //   std::cout << __FUNCTION__ << n.first << "," << n.second << std::endl;
  // }
  return neighbors;
}
