#include "RobotState.h"

RobotState::RobotState() {}

RobotState::~RobotState() {}

void RobotState::init(double battery, Position position) {
  battery_ = max_battery_ = battery;
  steps_to_full_charge_ = 20;
  robot_pos_ = position;
}

double RobotState::maxBattery() const { return max_battery_; }

double RobotState::battery() const { return battery_; }

void RobotState::step(Step stepDiretion) {
  if (battery_)
    battery_--;
  else
    return; // TODO: handle error scenario
  robot_pos_ = robot_pos_.next(stepDiretion);
}

void RobotState::charge() {
  battery_ += max_battery_ / steps_to_full_charge_;
  battery_ = std::min(battery_, max_battery_);
}

Position RobotState::getPosition() const { return robot_pos_; }
