#pragma once

#include "../../Common/WallsSensor.h"
#include "../../Common/common_types.h"
#include "House.h"
#include "RobotState.h"
#include "Utils.h"

#include <iostream>
#include <memory>

class WallsSensorImpl : public WallsSensor {
private:
  const House *house_;
  const RobotState *robot_;

public:
  WallsSensorImpl();
  WallsSensorImpl(const House &house, const RobotState &robot);
  bool isWall(Direction d) const override;
};
