#include "WallsSensorImpl.h"

WallsSensorImpl::WallsSensorImpl(const House &house, const RobotState &robot)
    : house_(&house), robot_(&robot) {}

bool WallsSensorImpl::isWall(Direction direction) const {

  return house_->isWall(
      robot_->getPosition().next(static_cast<Step>(direction)));
}
