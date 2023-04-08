#include "BatteryMeterImpl.h"

BatteryMeterImpl::BatteryMeterImpl(const RobotState &robotState)
    : robot_(&robotState) {}

std::size_t BatteryMeterImpl::getBatteryState() const {
  return robot_->battery();
}
