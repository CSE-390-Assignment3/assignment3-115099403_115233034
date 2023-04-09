#pragma once

#include "../../Common/DirtSensor.h"
#include "../../Common/common_types.h"
#include "House.h"
#include "RobotState.h"

#include <memory>

class DirtSensorImpl : public DirtSensor {
private:
  const House *house_;
  const RobotState *robot_;

public:
  DirtSensorImpl(const House &house, const RobotState &robot);
  int dirtLevel() const override;
};
