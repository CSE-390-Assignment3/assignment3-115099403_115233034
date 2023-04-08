#include "DirtSensorImpl.h"

DirtSensorImpl::DirtSensorImpl(const House &house, const RobotState &robot)
    : house_(&house), robot_(&robot) {}

int DirtSensorImpl::dirtLevel() const {
  return house_->dirtLevel(robot_->getPosition());
}
