#pragma once

#include "../../Common/common_types.h"
#include "Utils.h"

#include <ostream>
#include <string>
#include <vector>

class House {
private:
  std::vector<std::vector<int>> data_;
  Position dock_pos_;
  double total_dirt_ = 0;

public:
  House() {}
  Position getDockPos() const;
  int clean(const Position &position);
  double totDirt() const;
  int dirtLevel(const Position &position) const;
  bool isWall(const Position &position) const;
  void init(std::vector<std::vector<int>> &);
  friend std::ostream &operator<<(std::ostream &out, const House &h);
};
