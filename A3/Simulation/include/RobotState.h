#pragma once

#include "../../Common/common_types.h"
#include "Utils.h"

#include <algorithm>

class RobotState {
private:
  double max_battery_;
  double battery_ = 0;
  int steps_to_full_charge_ = MAX_STEPS_CHARGE;
  Position robot_pos_;

public:
  RobotState();
  ~RobotState();
  void init(double battery, Position position);
  double maxBattery() const;
  double battery() const;
  void step(Step stepDiretion);
  void charge();

  Position getPosition() const;
};
