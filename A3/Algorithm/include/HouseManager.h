#pragma once

#include "types.h"

#include <map>
#include <stack>

class HouseManager {
private:
  std::map<std::pair<int, int>, int> percieved_house_;
  std::map<std::pair<int, int>, bool> unexplored_points_;

  long long total_dirt_;

public:
  HouseManager();
  // @todo add const

  int total_dirt() const { return total_dirt_; }

  bool exists(const Pos pos) const;
  int dirt(const Pos pos);
  void setDirt(const Pos pos, int level);
  bool isWall(const Pos pos);
  bool isDock(const Pos pos);
  void clean(const Pos pos);
  // update and clean
  void clean(const Pos pos, int dirt);

  bool isUnexploredEmpty();
  bool isUnexplored(const Pos pos);
  // check and erase
  void eraseUnexplored(const Pos pos);

  // void updateNeighbors();
  void updateNeighbor(Direction dir, Pos position, bool isWall);

  std::stack<Direction> getShortestPath(std::pair<int, int> src,
                                        std::pair<int, int> dst,
                                        bool search = false);

  std::vector<std::pair<int, int>> neighbors(std::pair<int, int> point);
};
